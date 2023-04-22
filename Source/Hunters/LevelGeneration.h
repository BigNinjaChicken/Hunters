#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomOption.h"
#include "RoomPlacement.h"

#include "LevelGeneration.generated.h"

UCLASS()
class HUNTERS_API ALevelGeneration : public AActor
{
    GENERATED_BODY()

public:
    ALevelGeneration();

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
    void CacheAllRoomOptions();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation")
    int LeftRoomsPlaced = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation")
    int RightRoomsPlaced = 0;

    UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true), Category = "Generation")
    int RoomAmount = 50;

    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
	// class ARoomPlacement *RoomPlacementActor;

protected:
    virtual void BeginPlay() override;

    void PlaceRoom(ARoomOption *RoomOption);

public:
    virtual void Tick(float DeltaTime) override;
};
