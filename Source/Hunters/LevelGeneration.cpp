// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGeneration.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
ALevelGeneration::ALevelGeneration()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

void ALevelGeneration::CacheAllRoomOptions()
{
    TArray<AActor *> Rooms;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomOption::StaticClass(), Rooms);

    TArray<AActor *> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    for (AActor *RoomActor : Rooms)
    {
        ARoomOption *RoomOption = Cast<ARoomOption>(RoomActor);

        RoomOption->AllRooms.Reset();

        UBoxComponent *BoxComponent = RoomOption->SelectionBoxComponent;
        FVector BoxMin = BoxComponent->GetComponentLocation() - BoxComponent->GetScaledBoxExtent();
        FVector BoxMax = BoxComponent->GetComponentLocation() + BoxComponent->GetScaledBoxExtent();
        for (AActor *Actor : FoundActors)
        {
            FVector ActorLocation = Actor->GetActorLocation();
            FString ActorName = Actor->GetName();

            if (ActorLocation.X > BoxMin.X && ActorLocation.X < BoxMax.X &&
                ActorLocation.Y > BoxMin.Y && ActorLocation.Y < BoxMax.Y &&
                ActorLocation.Z > BoxMin.Z && ActorLocation.Z < BoxMax.Z &&
                Actor != RoomOption)
            {
                UE_LOG(LogTemp, Warning, TEXT("Actor %s is overlapping SelectionBoxComponent, Location: (%f, %f, %f)"), *ActorName, ActorLocation.X, ActorLocation.Y, ActorLocation.Z);
                RoomOption->AllRooms.Add(Actor);
            }
        }
    }
}

// Called when the game starts or when spawned
void ALevelGeneration::BeginPlay()
{
    Super::BeginPlay();

    LeftRoomsPlaced = 0;
    RightRoomsPlaced = 0;

    for (int i = RoomAmount; i > 0; i--)
    {
        AActor *RandomRoom = nullptr;
        do
        {
            TArray<AActor *> Rooms;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomOption::StaticClass(), Rooms);

            if (Rooms.Num() == 0)
            {
                UE_LOG(LogTemp, Error, TEXT("No actors of class ARoomOption found"));
                return;
            }

            int32 RandomIndex = FMath::RandRange(0, Rooms.Num() - 1);
            RandomRoom = Rooms[RandomIndex];

            if ((LeftRoomsPlaced == 1 && Cast<ARoomOption>(RandomRoom)->RoomType == ERoomType::Left) ||
                (RightRoomsPlaced == 1 && Cast<ARoomOption>(RandomRoom)->RoomType == ERoomType::Right))
            {
                RandomRoom = nullptr;
            }
        } while (RandomRoom == nullptr);

        PlaceRoom(Cast<ARoomOption>(RandomRoom));
    }
}

void ALevelGeneration::PlaceRoom(ARoomOption *RoomOption)
{
    float Rotation = (LeftRoomsPlaced * -90.0f) + (RightRoomsPlaced * 90.0f);

    FVector RoomStartLocation = RoomOption->GetActorLocation() + RoomOption->StartLocation;
    FVector RoomEndOffset = RoomOption->EndLocation - RoomOption->StartLocation;

    // Spawn the actor with the specified transform
    ARoomPlacement *RoomPlacementActor = GetWorld()->SpawnActor<ARoomPlacement>(ARoomPlacement::StaticClass());

    if (RoomPlacementActor == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn ARoomPlacement actor"));
        return;
    }

    RoomPlacementActor->SetActorLocation(RoomStartLocation);

    for (AActor *RoomActor : RoomOption->AllRooms)
    {
        // Create an exact clone of the RoomActor using the Duplicate function
        // AActor *RoomActorClone = Cast<AActor>(StaticDuplicateObject(RoomActor, RoomPlacementActor, NAME_None));

        FActorSpawnParameters Parameters;
        Parameters.Template = RoomActor;
        AActor *RoomActorClone = GetWorld()->SpawnActor<AActor>(RoomActor->GetClass(), Parameters);

        // Attach the clone to RoomPlacementActor
        RoomActorClone->AttachToActor(RoomPlacementActor, FAttachmentTransformRules::KeepWorldTransform);
    }

    RoomPlacementActor->SetActorLocation(GetActorLocation());
    RoomPlacementActor->GetRootComponent()->SetRelativeRotationExact(FRotator(0.0f, Rotation, 0.0f));
    // RoomPlacementActor->getr->SetWorldRotation(FRotator(0.0f, 0.0f, Rotation));
    
    GetRootComponent()->SetRelativeRotationExact(FRotator(0.0f, Rotation, 0.0f));
    AddActorLocalOffset(RoomEndOffset);

    if (RoomOption->RoomType == ERoomType::Right)
    {
        if (LeftRoomsPlaced == 1) {
            LeftRoomsPlaced--;
        } else {
            RightRoomsPlaced++;
        }
    }
    else if (RoomOption->RoomType == ERoomType::Left)
    {
         if (RightRoomsPlaced == 1) {
            RightRoomsPlaced--;
        } else {
            LeftRoomsPlaced++;
        }
    }
}

// Called every frame
void ALevelGeneration::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
