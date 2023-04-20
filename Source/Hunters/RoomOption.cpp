// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomOption.h"

#include "Components/BoxComponent.h"

// Sets default values
ARoomOption::ARoomOption()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a box component
	RoomSelectionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomSelection"));

	// Set the scale of the box component
	RoomSelectionBoxComponent->SetRelativeScale3D(FVector(3, 3, 3));

	RoomSelectionBoxComponent->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void ARoomOption::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoomOption::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

