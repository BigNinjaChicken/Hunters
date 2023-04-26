#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomOption.h"
#include "RoomPlacement.h"

#include "LevelGeneration.generated.h"

/**
 *
 */
UCLASS()
class HUNTERS_API ALevelGeneration : public AActor
{
    GENERATED_BODY()

public:
    /** Default constructor */
    ALevelGeneration();
    /** Cache all the possible room options */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
    void CacheAllRoomOptions();

    /** Get all the possible room options */
    TArray<ARoomOption *> GetAllRoomOptions();

    /** Get all actors */
    TArray<AActor *> GetAllActors();

    /** Check if actor overlaps selection box */
    bool ActorOverlapsSelectionBox(AActor *Actor, ARoomOption *RoomOption, FVector BoxMin, FVector BoxMax);

    /** Amount of rooms placed on the left side */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation")
    int LeftRoomsPlaced = 0;

    /** Amount of rooms placed on the right side */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation")
    int RightRoomsPlaced = 0;

    /** Amount of rooms to spawn */
    UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true), Category = "Generation")
    int RoomAmount = 50;

    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
    // class ARoomPlacement* RoomPlacementActor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
    class ANavMeshBoundsVolume* NavMeshBoundsVolume;

    /** Timer handle to start the game */
    UPROPERTY()
    FTimerHandle StartGameTimerHandle;

    /** Start the game */
    void StartGame();

    /** Place a room */
    void PlaceRoom(ARoomOption *RoomOption, int i);

    /** Duplicate an actor */
    AActor *DuplicateActor(AActor *OriginalActor);

    /** Actor to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    TSubclassOf<AActor> ActorToSpawn;

    /** Enemy actor to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    TSubclassOf<AActor> EnemyActorToSpawn;

protected:
    virtual void BeginPlay() override;
    /** Choose a random room */
    AActor *ChooseRandomRoom();

    /** Spawn actors */
    void SpawnActors(TSubclassOf<AActor> InputActor, ARoomOption *Room);

public:
    virtual void Tick(float DeltaTime) override;
};