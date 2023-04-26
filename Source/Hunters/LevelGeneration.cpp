// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGeneration.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "TimerManager.h"
#include "PassengerCharacter.h"
#include "AIConductor.h"
#include "AIController.h"
#include "EngineUtils.h"

// Sets default values
ALevelGeneration::ALevelGeneration()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

void ALevelGeneration::CacheAllRoomOptions()
{
    TArray<ARoomOption *> roomOptions = GetAllRoomOptions();
    TArray<AActor *> actors = GetAllActors();

    for (ARoomOption *roomOption : roomOptions)
    {
        roomOption->RoomActors.Reset();

        UBoxComponent *selectionBox = roomOption->SelectionBoxComponent;
        FVector boxMin = selectionBox->GetComponentLocation() - selectionBox->GetScaledBoxExtent();
        FVector boxMax = selectionBox->GetComponentLocation() + selectionBox->GetScaledBoxExtent();

        for (AActor *actor : actors)
        {
            if (ActorOverlapsSelectionBox(actor, roomOption, boxMin, boxMax))
            {
                FString actorName = actor->GetName();
                FVector actorLocation = actor->GetActorLocation();
                // UE_LOG(LogTemp, Warning, TEXT("Actor %s is overlapping SelectionBoxComponent, Location: (%f, %f, %f)"), *actorName, actorLocation.X, actorLocation.Y, actorLocation.Z);
                roomOption->RoomActors.Add(actor);
            }
        }
    }
}

TArray<ARoomOption *> ALevelGeneration::GetAllRoomOptions()
{
    TArray<AActor *> roomOptionActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomOption::StaticClass(), roomOptionActors);
    TArray<ARoomOption *> roomOptions;
    for (AActor *actor : roomOptionActors)
    {
        roomOptions.Add(Cast<ARoomOption>(actor));
    }
    return roomOptions;
}

TArray<AActor *> ALevelGeneration::GetAllActors()
{
    TArray<AActor *> actors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
    return actors;
}

bool ALevelGeneration::ActorOverlapsSelectionBox(AActor *actor, ARoomOption *roomOption, FVector boxMin, FVector boxMax)
{
    if (actor == roomOption)
    {
        return false;
    }

    FVector actorLocation = actor->GetActorLocation();

    return actorLocation.X > boxMin.X && actorLocation.X < boxMax.X &&
           actorLocation.Y > boxMin.Y && actorLocation.Y < boxMax.Y &&
           actorLocation.Z > boxMin.Z && actorLocation.Z < boxMax.Z;
}

// Called when the game starts or when spawned
void ALevelGeneration::BeginPlay()
{
    Super::BeginPlay();

    SpawnAI();

    // Cache all room options for faster retrieval
    CacheAllRoomOptions();

    // Initialize variables to track the number of rooms placed on each side
    LeftRoomsPlaced = 0;
    RightRoomsPlaced = 0;

    // Generate the level by placing rooms
    for (int RoomIndex = RoomAmount; RoomIndex > 0; RoomIndex--)
    {
        bool bShouldSpawnBoss = RoomIndex == RoomAmount / 2;

        // Boss Room Condition
        AActor *RandomRoom = ChooseRandomRoom(bShouldSpawnBoss);

        // Place the chosen room
        PlaceRoom(Cast<ARoomOption>(RandomRoom), RoomIndex, bShouldSpawnBoss);
    }

    // Start the game after a delay of 15 seconds
    GetWorldTimerManager().SetTimer(StartGameTimerHandle, this, &ALevelGeneration::StartGame, 15.0f, false);
}

void ALevelGeneration::SpawnAI()
{
    if (!PassengerCharacterBlueprint)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Inputed PassengerCharacter"));
        return;
    }

    if (!AIConductorBlueprint)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Inputed AIConductorBlueprint"));
        return;
    }

    TArray<AActor *> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomOption::StaticClass(), FoundActors);

    TArray<ARoomOption *> RoomOptions;
    for (AActor *Actor : FoundActors)
    {
        ARoomOption *RoomOption = Cast<ARoomOption>(Actor);
        if (RoomOption != nullptr)
        {
            RoomOptions.Add(RoomOption);
        }
    }

    for (ARoomOption *Room : RoomOptions)
    {
        // Spawn PassengerCharacterBlueprint
        const int32 AmountAISpawning = FMath::RandRange(1, Room->PassengerSpawnLocations.Num());

        for (int i = 0; i < AmountAISpawning; i++)
        {
            const int32 SpawnIndex = FMath::RandRange(0, Room->PassengerSpawnLocations.Num() - 1);
            const FVector SpawnLocation = Room->PassengerSpawnLocations[SpawnIndex] + Room->GetActorLocation();
            const FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

            APassengerCharacter *Passenger = GetWorld()->SpawnActor<APassengerCharacter>(PassengerCharacterBlueprint, SpawnLocation, SpawnRotation);
        }

        // Spawn AIConductorBlueprint
        const FVector SpawnLocation = Room->ConductorSpawnLocation + Room->GetActorLocation();
        const FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

        AAIConductor *Passenger = GetWorld()->SpawnActor<AAIConductor>(AIConductorBlueprint, SpawnLocation, SpawnRotation);
    }
}

AActor *ALevelGeneration::ChooseRandomRoom(bool bIsBossRoom)
{
    AActor *RandomRoom = nullptr;
    // Retrieve all available room options
    TArray<AActor *> RoomOptions;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomOption::StaticClass(), RoomOptions);

    // Handle the case where no room options are found
    if (RoomOptions.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No actors of class ARoomOption found"));
        return nullptr;
    }

    do
    {
        if (bIsBossRoom)
        {
            for (AActor *Room : RoomOptions)
            {
                if (Cast<ARoomOption>(Room)->RoomType == ERoomType::Straight)
                {
                    RandomRoom = Room;
                    break;
                }
            }
        }
        else
        {
            // Choose a random room from the available options
            int32 RandomIndex = FMath::RandRange(0, RoomOptions.Num() - 1);
            RandomRoom = RoomOptions[RandomIndex];

            // Ensure that there is only one left room and one right room
            if ((LeftRoomsPlaced == 1 && Cast<ARoomOption>(RandomRoom)->RoomType == ERoomType::Left) ||
                (RightRoomsPlaced == 1 && Cast<ARoomOption>(RandomRoom)->RoomType == ERoomType::Right))
            {
                RandomRoom = nullptr;
            }
        }
    } while (RandomRoom == nullptr);

    return RandomRoom;
}

void ALevelGeneration::StartGame()
{
}

void ALevelGeneration::PlaceRoom(ARoomOption *RoomOption, int RoomIndex, bool bShouldSpawnBoss)
{
    if (!RoomOption)
    {
        UE_LOG(LogTemp, Error, TEXT("Nullptr passed as RoomOption parameter to PlaceRoom function"));
        return;
    }

    float Rotation = (LeftRoomsPlaced * -90.0f) + (RightRoomsPlaced * 90.0f);

    FVector RoomStartLocation = RoomOption->GetActorLocation() + RoomOption->StartLocation;
    FVector RoomEndOffset = RoomOption->EndLocation - RoomOption->StartLocation;

    ARoomPlacement *RoomPlacementActor = GetWorld()->SpawnActor<ARoomPlacement>(ARoomPlacement::StaticClass());

    if (!RoomPlacementActor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn ARoomPlacement"));
        return;
    }

    RoomPlacementActor->SetActorLocation(RoomStartLocation);

    for (AActor *RoomActor : RoomOption->RoomActors)
    {
        if (!RoomActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to spawn RoomActor"));
            continue;
        }

        if (Cast<AAIConductor>(RoomActor))
        {
            if (!bShouldSpawnBoss)
            {
                continue;
            }
        }

        AActor *RoomActorClone = DuplicateActor(RoomActor);

        if (RoomActorClone)
        {
            RoomActorClone->AttachToActor(RoomPlacementActor, FAttachmentTransformRules::KeepWorldTransform);
        }
    }

    RoomPlacementActor->SetActorLocation(GetActorLocation());
    RoomPlacementActor->GetRootComponent()->SetRelativeRotationExact(FRotator(0.0f, Rotation, 0.0f));
    GetRootComponent()->SetRelativeRotationExact(FRotator(0.0f, Rotation, 0.0f));
    AddActorLocalOffset(RoomEndOffset);

    if (RoomOption->RoomType == ERoomType::Right)
    {
        if (LeftRoomsPlaced == 1)
        {
            LeftRoomsPlaced = 0;
        }
        else
        {
            RightRoomsPlaced++;
        }
    }
    else if (RoomOption->RoomType == ERoomType::Left)
    {
        if (RightRoomsPlaced == 1)
        {
            RightRoomsPlaced = 0;
        }
        else
        {
            LeftRoomsPlaced++;
        }
    }
}

AActor *ALevelGeneration::DuplicateActor(AActor *OriginalActor)
{
    if (!OriginalActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Nullptr passed as original actor."));
        return nullptr;
    }

    UWorld *World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not retrieve valid world."));
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Template = OriginalActor;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    FVector SpawnLocation = FVector(0.0f, 0.0f, 0.0f);
    FRotator SpawnRotation = FRotator::ZeroRotator;

    AActor *NewActor = World->SpawnActor<AActor>(OriginalActor->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
    if (!NewActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn duplicate actor."));
        return nullptr;
    }

    return NewActor;
}

// Called every frame
void ALevelGeneration::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
