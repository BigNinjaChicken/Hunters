// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomOption.generated.h"

UCLASS()
class HUNTERS_API ARoomOption : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoomOption();

    UPROPERTY(VisibleAnywhere, VisibleAnywhere, Category = SelectionArea)
	class UBoxComponent *RoomSelectionBoxComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};