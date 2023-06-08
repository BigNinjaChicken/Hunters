// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainEnemyCharacter.generated.h"

UCLASS()
class HUNTERS_API AMainEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	class AMyAIController* MainEnemyController;

public:
	void StartChase();

	bool bHasStarted = false;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class USoundCue *BeatheSoundCue;

	UAudioComponent *audioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Generation")
    class UBoxComponent* BoxComponent;

	float SpeedIncreaseAmount = 0.01f;

	void OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
};
