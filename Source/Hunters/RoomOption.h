#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomOption.generated.h"

UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Straight UMETA(DisplayName = "Straight"),
    Right UMETA(DisplayName = "Right"),
    Left UMETA(DisplayName = "Left")
};

UCLASS()
class HUNTERS_API ARoomOption : public AActor
{
    GENERATED_BODY()

public:
    ARoomOption();

    UPROPERTY(EditAnywhere, Category = "Generation", meta = (MakeEditWidget = true))
    FVector BoxExtent = FVector(100.0f, 100.0f, 100.0f);

    UPROPERTY(EditAnywhere, Category = "Generation", meta = (MakeEditWidget = true))
    FVector StartLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Generation", meta = (MakeEditWidget = true))
    FVector EndLocation = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, Category = "Generation")
    class UBoxComponent* SelectionBoxComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (DisplayName = "Room Type"))
    ERoomType RoomType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
    TArray<AActor*> AllRooms;

protected:
    virtual void OnConstruction(const FTransform& Transform) override;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
