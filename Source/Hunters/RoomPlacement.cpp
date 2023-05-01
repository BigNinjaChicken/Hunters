// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomPlacement.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/PostProcessVolume.h"
#include "PlayerCharacter.h"

// Sets default values
ARoomPlacement::ARoomPlacement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent *RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
}

// Called when the game starts or when spawned
void ARoomPlacement::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARoomPlacement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (RoomEndLocation.IsZero())
	// {
	// 	return;
	// }
	
	// APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// if (!PlayerController)
	// {
	// 	return;
	// }

	// APlayerCharacter *PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
	// if (!PlayerCharacter)
	// {
	// 	return;
	// }

	// // Check if the player character has entered the boss room
	// if (!bIsPlayerInBossRoom && FVector::DistSquared(PlayerCharacter->GetActorLocation(), GetActorLocation()) < FMath::Square(DetectionRange))
	// {
	// 	// Find all Post Process Volumes in the world
	// 	TArray<AActor *> FoundActors;
	// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

	// 	APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);

	// 	if (PostProcessVolume)
	// 	{
	// 		SavedVignetteIntensity = PostProcessVolume->Settings.VignetteIntensity;
	// 		PostProcessVolume->Settings.VignetteIntensity = BossRoomVignetteIntensity;
	// 	}

	// 	SavedMaxWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
	// 	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = BossRoomMaxWalkSpeed;

	// 	// Set the flag to indicate that the player character is now in the boss room
	// 	bIsPlayerInBossRoom = true;
	// }
	// // Check if the player character has left the boss room
	// else if (bIsPlayerInBossRoom && FVector::DistSquared(PlayerCharacter->GetActorLocation(), RoomEndLocation) < FMath::Square(DetectionRange))
	// {
	// 	// Find all Post Process Volumes in the world
	// 	TArray<AActor *> FoundActors;
	// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

	// 	APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);
	// 	if (PostProcessVolume)
	// 	{
	// 		PostProcessVolume->Settings.VignetteIntensity = SavedVignetteIntensity;
	// 	}
	// 	if (PlayerCharacter)
	// 	{
	// 		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = SavedMaxWalkSpeed;
	// 	}

	// 	// Set the flag to indicate that the player character has left the boss room
	// 	bIsPlayerInBossRoom = false;
	// }
}

void ARoomPlacement::CreateBossRoom(FVector RoomEndOffset)
{
	RoomEndLocation = RoomEndOffset;
}