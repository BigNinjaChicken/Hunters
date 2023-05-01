// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"

#include "AIConductor.generated.h"

USTRUCT()
struct FSpeechOptions
{
	GENERATED_BODY()

    UPROPERTY()
    TMap<FString, float> Entries;
};

UCLASS()
class HUNTERS_API AAIConductor : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIConductor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class AMyAIController *PassengerController;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	class UWidgetComponent *ButtonPromptWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	class UBoxComponent *TriggerVolume;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float SavedMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float SavedVignetteIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float BossRoomMaxWalkSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float BossRoomVignetteIntensity = 1.6f;

public:
	TArray<FSpeechOptions> SpeechOptions;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class USoundCue *TalkSoundCue;
};
