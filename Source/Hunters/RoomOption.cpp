// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomOption.h"

#include "Components/BoxComponent.h"

// Sets default values
ARoomOption::ARoomOption()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SelectionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionBoxComponent"));
    RootComponent = SelectionBoxComponent;
}

void ARoomOption::OnConstruction(const FTransform &Transform)
{
    Super::OnConstruction(Transform);
    
    SelectionBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SelectionBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    SelectionBoxComponent->SetLineThickness(10.0f);

    // Set the size of the box selection area based on the BoxExtent variable
    SelectionBoxComponent->SetBoxExtent(BoxExtent);

    PassengerSpawnLocations.SetNum(SpawnLocationAmount);
    for (int32 i = 0; i < SpawnLocationAmount; i++) {
        if (PassengerSpawnLocations[i].IsZero()) {
            PassengerSpawnLocations[i] = FVector(100.0f, 100.0f, 100.0f);
        }
    }
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
