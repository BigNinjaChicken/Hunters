// Fill out your copyright notice in the Description page of Project Settings.

#include "AIConductor.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"
#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationInvokerComponent.h"
#include "Engine/PostProcessVolume.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AAIConductor::AAIConductor()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ButtonPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ButtonPromptWidget"));
    ButtonPromptWidget->SetupAttachment(RootComponent);
    ButtonPromptWidget->SetVisibility(false);

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->SetupAttachment(RootComponent);
    TriggerVolume->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AAIConductor::BeginPlay()
{
    Super::BeginPlay();

    // Spawn the AI controller and attach it to the AI character
    PassengerController = GetWorld()->SpawnActor<AMyAIController>(AMyAIController::StaticClass());
    if (PassengerController)
    {
        PassengerController->Possess(this);
    }

    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AAIConductor::OnOverlapBegin);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AAIConductor::OnOverlapEnd);

    SpeechOptions.Reset();

    // Create some FSpeechOptions objects
    FSpeechOptions option1;
    option1.Entries.Add("Time", 0.5f);
    option1.Entries.Add("To", 1.5f);
    option1.Entries.Add("Ride", 2.0f);

    FSpeechOptions option2;
    option2.Entries.Add("Let", 0.5f);
    option2.Entries.Add("Us", 1.5f);
    option2.Entries.Add("Begin", 2.5f);

    FSpeechOptions option3;
    option3.Entries.Add("Tickets", 0.5f);
    option3.Entries.Add("Pl", 1.5f);
    option3.Entries.Add("Please", 2.0f);

    FSpeechOptions option4;
    option4.Entries.Add("Where", 0.5f);
    option4.Entries.Add("are", 1.5f);
    option4.Entries.Add("you", 2.5f);
    option4.Entries.Add("headed", 2.5f);

    FSpeechOptions option5;
    option5.Entries.Add("Where", 0.5f);
    option5.Entries.Add("are", 1.5f);
    option5.Entries.Add("you", 2.0f);
    option5.Entries.Add("headed", 2.5f);

    FSpeechOptions option6;
    option6.Entries.Add("Are", 0.5f);
    option6.Entries.Add("you", 1.5f);
    option6.Entries.Add("enjoying", 2.5f);
    option6.Entries.Add("the", 3.0f);
    option6.Entries.Add("ride", 3.5f);

    FSpeechOptions option7;
    option7.Entries.Add("I", 0.5f);
    option7.Entries.Add("love", 1.5f);
    option7.Entries.Add("trains", 2.5f);

    FSpeechOptions option8;
    option8.Entries.Add("I", 0.5f);
    option8.Entries.Add("wonder", 1.5f);
    option8.Entries.Add("where", 2.0f);
    option8.Entries.Add("the", 3.0f);
    option8.Entries.Add("con", 3.5f);
    option8.Entries.Add("ductor", 4.0f);
    option8.Entries.Add("went", 4.5f);

    FSpeechOptions option9;
    option9.Entries.Add("Have", 0.5f);
    option9.Entries.Add("you", 1.5f);
    option9.Entries.Add("seen", 2.5f);
    option9.Entries.Add("the", 3.0f);
    option9.Entries.Add("dining", 3.5f);
    option9.Entries.Add("car", 4.0f);

    // Add the options to the TArray
    SpeechOptions.Add(option1);
    SpeechOptions.Add(option2);
    SpeechOptions.Add(option3);
    SpeechOptions.Add(option4);
    SpeechOptions.Add(option5);
    SpeechOptions.Add(option6);
    SpeechOptions.Add(option7);
    SpeechOptions.Add(option8);
    SpeechOptions.Add(option9);
}

// Called every frame
void AAIConductor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAIConductor::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor->IsA(APlayerCharacter::StaticClass()))
    {
        ButtonPromptWidget->SetVisibility(true);

        // Find all Post Process Volumes in the world
        // TArray<AActor *> FoundActors;
        // UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

        // APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);

        // if (PostProcessVolume)
        // {
        //     SavedVignetteIntensity = PostProcessVolume->Settings.VignetteIntensity;
        //     PostProcessVolume->Settings.VignetteIntensity = BossRoomVignetteIntensity;
        // }

        // APlayerCharacter *PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
        // SavedMaxWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
        // PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = BossRoomMaxWalkSpeed;
    }
}

void AAIConductor::OnOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(APlayerCharacter::StaticClass()))
    {
        ButtonPromptWidget->SetVisibility(false);

        // Find all Post Process Volumes in the world
        // TArray<AActor *> FoundActors;
        // UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

        // APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);
        // if (PostProcessVolume)
        // {
        //     PostProcessVolume->Settings.VignetteIntensity = SavedVignetteIntensity;
        // }

        // APlayerCharacter *PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
        // if (PlayerCharacter)
        // {
        //     PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = SavedMaxWalkSpeed;
        // }
    }
}