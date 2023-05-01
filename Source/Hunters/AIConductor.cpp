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
		TArray<AActor *> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

		APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);

		if (PostProcessVolume)
		{
			SavedVignetteIntensity = PostProcessVolume->Settings.VignetteIntensity;
			PostProcessVolume->Settings.VignetteIntensity = BossRoomVignetteIntensity;
		}

        APlayerCharacter *PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		SavedMaxWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = BossRoomMaxWalkSpeed;
    }
}

void AAIConductor::OnOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(APlayerCharacter::StaticClass()))
    {
        ButtonPromptWidget->SetVisibility(false);

        // Find all Post Process Volumes in the world
		TArray<AActor *> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

		APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);
		if (PostProcessVolume)
		{
			PostProcessVolume->Settings.VignetteIntensity = SavedVignetteIntensity;
		}

        APlayerCharacter *PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = SavedMaxWalkSpeed;
		}
    }
}