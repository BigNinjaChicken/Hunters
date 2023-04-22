// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomPlacement.h"

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
}
