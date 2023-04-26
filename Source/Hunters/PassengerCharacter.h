#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "PassengerCharacter.generated.h"

UCLASS()
class HUNTERS_API APassengerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APassengerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	class AMyAIController *PassengerController;

	FVector TargetLocation;
};
