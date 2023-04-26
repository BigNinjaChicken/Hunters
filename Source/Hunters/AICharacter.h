// AICharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "AICharacter.generated.h"

UENUM()
enum class EAIState : uint8
{
    StandingStill,
    MovingToPlayer
};

UCLASS()
class HUNTERS_API AAICharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAICharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent *AIPerceptionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    EAIState AIState;

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor *Actor, FAIStimulus Stimulus);

    UFUNCTION()
    void Explode();

    UPROPERTY()
    FTimerHandle ExplosionTimerHandle;

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay();

    UPROPERTY()
    class AMyAIController *AIController;

    UPROPERTY()
    class AActor *Player;
};