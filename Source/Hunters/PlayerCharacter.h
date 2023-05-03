// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/World.h"
#include "AIConductor.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"

#include "PlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UENUM(BlueprintType)
enum class EGrade : uint8
{
	Perfect UMETA(DisplayName = "Perfect"),
	Good UMETA(DisplayName = "Good"),
	Bad UMETA(DisplayName = "Bad")
};

UCLASS()
class HUNTERS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent *Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext *DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext *ExtraMovementMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *TalkAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class USoundCue *TalkSoundCue;

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay();

    virtual void Tick(float DeltaSeconds) override;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EOS")
	TSubclassOf<UUserWidget> MainMenu;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue &Value);

	/** Called for looking input */
	void Look(const FInputActionValue &Value);

	void Sprint(const FInputActionValue &Value);

	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (Hidden))
	bool bIsSprinting;

	float DefaultMaxWalkingSpeed;

	UPROPERTY(EditAnywhere, Category = Input, meta = (ToolTip = "Amount multiplied by movement speed while sprinting is active."))
	float SprintingMultiplier = 1.5;

	void Talk(const FInputActionValue &Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent *InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent *GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent *GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

public:
	// Talking Minigame
	TArray<FHitResult> MultiSphereTrace();

	void StartTalkingMiniGame(AAIConductor *HitAIConductor);

	void RespondTalking();
	bool bCaptureInputs = false;
	bool bRespondPhase = false;
	bool bHasStartedTalking = false;
	bool bCanBeScored = false;
	TArray<float> CapturedInputTimes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Talking")
	TSubclassOf<UUserWidget> TalkingMiniGame;
	UUserWidget *TalkingMiniGameWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float SavedMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float SavedVignetteIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float BossRoomMaxWalkSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talking")
	float BossRoomVignetteIntensity = 1.6f;

	class UCanvasPanel *Canvas;
	class UBorder *Ball;
	class UHorizontalBox *HorizontalBox;
	TArray<TPair<FString, float>> AllLines;
	TArray<class UTextBlock *> TextBlocks;

	void ResetTalking();

public:
	// Intro Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intro")
	TSubclassOf<UUserWidget> ActUserWidget;
	UUserWidget *ActWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intro")
	TSubclassOf<UUserWidget> IntroUserWidget;
	UUserWidget *IntroWidget;
	UTextBlock *IntroText;

	TArray<FString> AllIntroText = {"", "You know the plan.", "Get to the Front as fast as possible.", "And don't let the conductor catch you."};

protected:
    class UTimelineComponent* IntroTimelineComponent;
 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intro")
    class UCurveFloat* FloatCurve;

	UFUNCTION()
    void IntroTimelineComponentCallback(float val);
    
    UFUNCTION()
    void IntroTimelineComponentFinishedCallback();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intro")
	class ULevelSequence* LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intro")
	TSubclassOf<UCameraShakeBase> CameraShakeBase;
};
