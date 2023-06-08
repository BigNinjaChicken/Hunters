// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MainEnemyCharacter.h"

// Sets default values
AMainEnemyCharacter::AMainEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComponent->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f));
}

// Called when the game starts or when spawned
void AMainEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMainEnemyCharacter::OnOverlapBegin);
}

void AMainEnemyCharacter::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        // OtherActor is a PlayerCharacter, do something with it
        PlayerCharacter->LoseDemo();
    }
}

// Called every frame
void AMainEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHasStarted)
	{
		return;
	}

	FNavLocation ResultLocation;
	APlayerCharacter *Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(Player->GetActorLocation(), 100.0f, ResultLocation))
	{
		MainEnemyController->MoveToLocation(ResultLocation.Location, 50.0f);
	}

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	characterMovement->MaxWalkSpeed += SpeedIncreaseAmount;
}

void AMainEnemyCharacter::StartChase()
{
	MainEnemyController = GetWorld()->SpawnActor<AMyAIController>(AMyAIController::StaticClass());
	if (MainEnemyController)
	{
		MainEnemyController->Possess(this);
		bHasStarted = true;

		audioComponent->SetSound(BeatheSoundCue);
		audioComponent->bAutoActivate = true;
	}
}