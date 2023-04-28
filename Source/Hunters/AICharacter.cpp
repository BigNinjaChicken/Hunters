// AICharacter.cpp
#include "AICharacter.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"
#include "MyAIController.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AAICharacter::AAICharacter()
{
    AIControllerClass = AMyAIController::StaticClass();

    // Set up AI Perception Component
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    UAISenseConfig_Sight *SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 3000.0f;
    SightConfig->LoseSightRadius = 3500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 180.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAICharacter::OnTargetPerceptionUpdated);
}

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();

    // Spawn the AI controller and attach it to the AI character
    AIController = GetWorld()->SpawnActor<AMyAIController>(AIControllerClass);
    if (AIController)
    {
        AIController->Possess(this);
    }
}

void AAICharacter::Tick(float DeltaSeconds)
{
    if (AIState == EAIState::MovingToPlayer)
    {
        AIController->MoveToActor(Player);
    }

    if (AIState == EAIState::StandingStill)
    {
        AIController->StopMovement();
    }
}

// MyAICharacter.cpp
void AAICharacter::OnTargetPerceptionUpdated(AActor *Actor, FAIStimulus Stimulus)
{
    if (!Actor->IsA<APlayerCharacter>())
    {
        UE_LOG(LogTemp, Warning, TEXT("OnTargetPerceptionUpdated: Invalid Actor or not of class APlayerCharacter"));
        return;
    }

    if (Stimulus.WasSuccessfullySensed()) // Player detected
    {
        Player = Actor;
        AIState = EAIState::MovingToPlayer;

        // Start the 5 second timer
        GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AAICharacter::Explode, 5.0f, false);
    }
    else // Player lost
    {
        AIState = EAIState::StandingStill;
    }
}

void AAICharacter::Explode()
{
    // Get all actors nearby
    TArray<AActor *> Actors;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerCharacter::StaticClass(), Actors);

    // Iterate over all nearby actors
    for (AActor *Actor : Actors)
    {
        // Check if the actor is a player character
        APlayerCharacter *PlayerCharacter = Cast<APlayerCharacter>(Actor);
        if (PlayerCharacter)
        {
            // Check if the player character is within 200 units of this AI character
            float Distance = FVector::Distance(GetActorLocation(), PlayerCharacter->GetActorLocation());
            if (Distance <= KillDistance)
            {
                // Destroy the player character
                PlayerCharacter->Destroy();
            }
        }
    }

    // Destroy the AI character
    Destroy();
}
