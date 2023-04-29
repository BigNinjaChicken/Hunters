// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomPlacement.generated.h"

UCLASS()
class HUNTERS_API ARoomPlacement : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoomPlacement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Called From Level Generation
	void CreateBossRoom(FVector RoomEndOffset);

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	FVector RoomEndLocation;

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	float SavedMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	float SavedVignetteIntensity;

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	float DetectionRange = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	float BossRoomMaxWalkSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	float BossRoomVignetteIntensity = 1.6f;

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	bool bIsPlayerInBossRoom = false;
};
