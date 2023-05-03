// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/PostProcessVolume.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"
#include "LevelSequencePlayer.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "Math/UnrealMathUtility.h"

#include "GameFramework/PlayerController.h" // for APlayerController class

//////////////////////////////////////////////////////////////////////////
// APlayerCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	// Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	IntroTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("IntroTimelineComponent"));
	ConductorTalkingTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("ConductorTalkingTimelineComponent"));
	PlayerTalkingTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("PlayerTalkingTimelineComponent"));
	OutroTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("OutroTimelineComponent"));
}

void APlayerCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	UCharacterMovementComponent *MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		DefaultMaxWalkingSpeed = MovementComponent->MaxWalkSpeed;
	}

	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		// Set the input mode to game only
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;

		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			Subsystem->AddMappingContext(ExtraMovementMappingContext, 1);
		}
	}

	if (IsLocallyControlled())
	{
		// create widget
		TalkingMiniGameWidget = CreateWidget<UUserWidget>(GetWorld(), TalkingMiniGame);
		if (TalkingMiniGameWidget)
		{
			TalkingMiniGameWidget->AddToViewport();
			TalkingMiniGameWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		ActWidget = CreateWidget<UUserWidget>(GetWorld(), ActUserWidget);
		if (ActWidget)
		{
			ActWidget->AddToViewport();
		}

		IntroWidget = CreateWidget<UUserWidget>(GetWorld(), IntroUserWidget);
		if (IntroWidget)
		{
			IntroWidget->AddToViewport();

			IntroText = Cast<UTextBlock>(IntroWidget->GetWidgetFromName(FName("IntroText")));
			if (IntroText)
			{
				if (IntroTimelineComponent)
				{
					// Bind the OnTimelineFloat function to the timeline's float track
					FOnTimelineFloat TimelineFloat;
					FOnTimelineEventStatic onTimelineFinishedCallback;
					TimelineFloat.BindUFunction(this, "IntroTimelineComponentCallback");
					onTimelineFinishedCallback.BindUFunction(this, "IntroTimelineComponentFinishedCallback");
					IntroTimelineComponent->AddInterpFloat(FloatCurve, TimelineFloat);
					IntroTimelineComponent->SetTimelineFinishedFunc(onTimelineFinishedCallback);
					const float IntroLength = 18.0f;
					IntroTimelineComponent->SetPlayRate(1.0f / IntroLength);
					IntroTimelineComponent->SetLooping(false);
					IntroTimelineComponent->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

					IntroTimelineComponent->PlayFromStart();
				}
			}
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (IntroTimelineComponent != NULL)
	// {
	// 	IntroTimelineComponent->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	// }
}

void APlayerCharacter::IntroTimelineComponentCallback(float interpolatedVal)
{
	// UKismetMathLibrary::NormalizeToRange(UKismetMathLibrary::NormalizeToRange(OriginalValue, OriginalMin, OriginalMax), NewMin, NewMax);
	int32 TextIndex = UKismetMathLibrary::MapRangeClamped(interpolatedVal, 0.0f, 1.0f, 0.0f, ((float)AllIntroText.Num()) - 0.001f);

	IntroText->SetText(FText::FromString(AllIntroText[TextIndex]));
}

void APlayerCharacter::IntroTimelineComponentFinishedCallback()
{
	IntroWidget->SetVisibility(ESlateVisibility::Collapsed);
	ActWidget->SetVisibility(ESlateVisibility::Collapsed);

	if (LevelSequence)
	{
		ALevelSequenceActor *LevelSequenceActor;
		ULevelSequencePlayer *LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, FMovieSceneSequencePlaybackSettings(), LevelSequenceActor);
		if (LevelSequencePlayer)
		{
			LevelSequencePlayer->Play();

			ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal;
			APlayerCameraManager *CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
			CameraManager->StartCameraShake(CameraShakeBase, 1.0f, PlaySpace, FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);

		EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Talk);
	}
}

void APlayerCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Sprint(const FInputActionValue &Value)
{
	UCharacterMovementComponent *MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No movement component found"));
		return;
	}

	if (bIsSprinting)
	{
		MovementComponent->MaxWalkSpeed = DefaultMaxWalkingSpeed;
	}
	else
	{
		MovementComponent->MaxWalkSpeed *= SprintingMultiplier;
	}

	bIsSprinting = !bIsSprinting;
}

void APlayerCharacter::Talk(const FInputActionValue &Value)
{
	if (!TalkSoundCue)
	{
		UE_LOG(LogTemp, Warning, TEXT("No sound cue component found"));
		return;
	}

	if (bRespondPhase)
	{
		AActor *SelfActor = GetOwner();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TalkSoundCue, SelfActor->GetActorLocation());

		if (bHasStartedTalking)
		{

			return;
		}

		return;
	}

	if (bCaptureInputs)
	{
		return;
	}

	AActor *SelfActor = GetOwner();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TalkSoundCue, SelfActor->GetActorLocation());

	TArray<FHitResult> OutHits = MultiSphereTrace();

	for (const FHitResult &Hit : OutHits)
	{
		AActor *HitActor = Hit.GetActor();
		if (!HitActor)
		{
			return;
		}

		AAIConductor *HitAIConductor = Cast<AAIConductor>(HitActor);
		if (!HitAIConductor)
		{
			return;
		}

		StartTalkingMiniGame(HitAIConductor);
		break;
	}
}

TArray<FHitResult> APlayerCharacter::MultiSphereTrace()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * 100.0f;
	float Radius = 50.0f;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor *> ActorsToIgnore;
	TArray<FHitResult> OutHits;

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true);

	return OutHits;
}

void APlayerCharacter::StartTalkingMiniGame(AAIConductor *HitAIConductor)
{
	int32 ArraySize = HitAIConductor->SpeechOptions.Num();
	if (ArraySize == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Speech Options"));
		return;
	}

	if (!HitAIConductor->TalkSoundCue)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Inputed TalkSoundCue"));
		return;
	}

	HorizontalBox = Cast<UHorizontalBox>(TalkingMiniGameWidget->GetWidgetFromName(FName("HorizontalBox")));
	if (!HorizontalBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("No HorizontalBox"));
		return;
	}

	Ball = Cast<UBorder>(TalkingMiniGameWidget->GetWidgetFromName(FName("Ball")));
	if (!Ball)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Ball"));
		return;
	}

	Canvas = Cast<UCanvasPanel>(TalkingMiniGameWidget->GetWidgetFromName(FName("CanvasPanel")));
	if (!Canvas)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Canvas"));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, ArraySize - 1);
	FSpeechOptions SpeechOption = HitAIConductor->SpeechOptions[RandomIndex];

	AllLines = SpeechOption.Entries.Array();
	float TotalValue = 0;
	// Create the text box widgets
	for (TPair<FString, float> line : AllLines)
	{
		FString Key = line.Key;
		float Value = line.Value;
		TotalValue += Value;

		// Create the text box widget
		UTextBlock *TextBox = NewObject<UTextBlock>(this);
		TextBox->SetAutoWrapText(true);
		FSlateFontInfo TextBoxFont = TextBox->GetFont();
		TextBoxFont.Size = 30;
		TextBox->SetFont(TextBoxFont);

		TextBox->SetText(FText::FromString(Key + " "));

		// Add the text box widget as a child of the horizontal box widget
		HorizontalBox->AddChild(TextBox);

		// Add the text box widget to the array
		TextBlocks.Add(TextBox);
	}

	TalkingMiniGameWidget->SetVisibility(ESlateVisibility::Visible);

	TArray<AActor *> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

	APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);

	if (PostProcessVolume)
	{
		SavedVignetteIntensity = PostProcessVolume->Settings.VignetteIntensity;
		PostProcessVolume->Settings.VignetteIntensity = BossRoomVignetteIntensity;
	}

	SavedMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BossRoomMaxWalkSpeed;

	bCaptureInputs = true;

	if (!ConductorTalkingTimelineComponent)
	{
		return;
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, [this, HitAIConductor]()
		{
			for (int index = 0; index < TextBlocks.Num(); ++index)
			{
				int c = TextBlocks.Num();
				int k = index;

				int center = 0;
				for (int i = 0; i < c; ++i) {
					center += TextBlocks[i]->GetDesiredSize().X;
				}
				center /= 2;

				int distance = center;
				for (int i = 0; i < k; ++i) {
					distance -= TextBlocks[i]->GetDesiredSize().X;
				}

				// Adjust the distance for the center of the inputted card index
				distance -= TextBlocks[k]->GetDesiredSize().X / 2;

				WordCenters.Add(distance * -1);
			}

			// Bind the OnTimelineFloat function to the timeline's float track
			FOnTimelineFloat TimelineFloat;
			FOnTimelineEventStatic onTimelineFinishedCallback;
			TimelineFloat.BindUFunction(this, "ConductorTalkingTimelineComponentCallback");
			onTimelineFinishedCallback.BindUFunction(this, "ConductorTalkingTimelineComponentFinishedCallback");
			ConductorTalkingTimelineComponent->AddInterpFloat(FloatCurve, TimelineFloat);
			ConductorTalkingTimelineComponent->SetTimelineFinishedFunc(onTimelineFinishedCallback);
			ConductorTalkingTimelineComponent->SetLooping(false);
			ConductorTalkingTimelineComponent->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

			ConductorTalkingTimelineComponent->SetPlayRate(1.0f / AllLines[ConductorTalkingIndex].Value);
			ConductorTalkingTimelineComponent->PlayFromStart(); },
		0.1f, false);
}

void APlayerCharacter::ConductorTalkingTimelineComponentCallback(float val)
{
	const TArray<UPanelSlot *> Panels = Canvas->GetSlots();
	for (UPanelSlot *Panel : Panels)
	{
		UCanvasPanelSlot *CanvasSlot = Cast<UCanvasPanelSlot>(Panel);
		if (!CanvasSlot)
		{
			continue;
		}

		UWidget *ChildWidget = CanvasSlot->Content;
		if (!ChildWidget)
		{
			continue;
		}

		if (!Cast<UBorder>(ChildWidget))
		{
			continue;
		}

		float ExponentialFunctionY = -100.0 * exp(-10.0 * pow(val - 0.5, 2.0));

		CanvasSlot->SetPosition(FVector2D(FMath::Lerp(WordCenters[ConductorTalkingIndex], WordCenters[ConductorTalkingIndex + 1], val), ExponentialFunctionY));
	}
}

void APlayerCharacter::ConductorTalkingTimelineComponentFinishedCallback()
{
	ConductorTalkingIndex++;
	if (WordCenters.Num() <= ConductorTalkingIndex + 1)
	{
		bRespondPhase = true;
		RespondTalking();
		return;
	}

	ConductorTalkingTimelineComponent->SetPlayRate(1.0f / (AllLines[ConductorTalkingIndex].Value - AllLines[ConductorTalkingIndex - 1].Value));
	ConductorTalkingTimelineComponent->PlayFromStart();
}

void APlayerCharacter::RespondTalking()
{
	bHasStartedTalking = true;

	FOnTimelineFloat TimelineFloat;
	FOnTimelineEventStatic onTimelineFinishedCallback;
	TimelineFloat.BindUFunction(this, "PlayerTalkingTimelineComponentCallback");
	onTimelineFinishedCallback.BindUFunction(this, "PlayerTalkingTimelineComponentFinishedCallback");
	PlayerTalkingTimelineComponent->AddInterpFloat(FloatCurve, TimelineFloat);
	PlayerTalkingTimelineComponent->SetTimelineFinishedFunc(onTimelineFinishedCallback);
	PlayerTalkingTimelineComponent->SetLooping(false);
	PlayerTalkingTimelineComponent->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	PlayerTalkingTimelineComponent->SetPlayRate(1.0f / 2.0f);
	bResetBall = true;
	PlayerTalkingTimelineComponent->PlayFromStart();
}

void APlayerCharacter::PlayerTalkingTimelineComponentCallback(float val)
{
	const TArray<UPanelSlot *> Panels = Canvas->GetSlots();
	for (UPanelSlot *Panel : Panels)
	{
		UCanvasPanelSlot *CanvasSlot = Cast<UCanvasPanelSlot>(Panel);
		if (!CanvasSlot)
		{
			continue;
		}

		UWidget *ChildWidget = CanvasSlot->Content;
		if (!ChildWidget)
		{
			continue;
		}

		if (!Cast<UBorder>(ChildWidget))
		{
			continue;
		}

		if (bResetBall)
		{
			float ExponentialFunctionY = -200.0 * exp(-10.0 * pow(val - 0.5, 2.0));
			CanvasSlot->SetPosition(FVector2D(FMath::Lerp(WordCenters[ConductorTalkingIndex], WordCenters[PlayerTalkingIndex], val), ExponentialFunctionY));
		}
		else
		{
			float ExponentialFunctionY = -100.0 * exp(-10.0 * pow(val - 0.5, 2.0));
			CanvasSlot->SetPosition(FVector2D(FMath::Lerp(WordCenters[PlayerTalkingIndex], WordCenters[PlayerTalkingIndex + 1], val), ExponentialFunctionY));
		}
	}
}

void APlayerCharacter::PlayerTalkingTimelineComponentFinishedCallback()
{
	if (!bResetBall)
	{
		PlayerTalkingIndex++;
	}

	if (WordCenters.Num() <= PlayerTalkingIndex + 1)
	{
		ResetTalking();
		return;
	}

	bResetBall = false;
	if (PlayerTalkingIndex - 1 >= 0)
	{
		PlayerTalkingTimelineComponent->SetPlayRate(1.0f / (AllLines[PlayerTalkingIndex].Value - AllLines[PlayerTalkingIndex - 1].Value));
	}
	else
	{
		PlayerTalkingTimelineComponent->SetPlayRate(1.0f / AllLines[PlayerTalkingIndex].Value);
	}
	PlayerTalkingTimelineComponent->PlayFromStart();
}

void APlayerCharacter::ResetTalking()
{
	TalkingMiniGameWidget->SetVisibility(ESlateVisibility::Collapsed);

	TArray<AActor *> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

	APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);

	if (PostProcessVolume)
	{
		PostProcessVolume->Settings.VignetteIntensity = SavedVignetteIntensity;
	}

	GetCharacterMovement()->MaxWalkSpeed = SavedMaxWalkSpeed;

	bCaptureInputs = false;
	bRespondPhase = false;
	bHasStartedTalking = false;
	bCanBeScored = false;
	bResetBall = false;

	HorizontalBox->ClearChildren();
	AllLines.Empty();
	TextBlocks.Empty();
	CapturedInputTimes.Empty();
	WordCenters.Empty();

	PlayerTalkingIndex = 0;
	ConductorTalkingIndex = 0;
}

void APlayerCharacter::EndDemo()
{
	if (OutroTimelineComponent)
	{
		IntroWidget = CreateWidget<UUserWidget>(GetWorld(), IntroUserWidget);
		if (IntroWidget)
		{
			IntroWidget->AddToViewport();

			IntroText = Cast<UTextBlock>(IntroWidget->GetWidgetFromName(FName("IntroText")));
		}

		// Bind the OnTimelineFloat function to the timeline's float track
		FOnTimelineFloat TimelineFloat;
		FOnTimelineEventStatic onTimelineFinishedCallback;
		TimelineFloat.BindUFunction(this, "OutroTimelineComponentCallback");
		onTimelineFinishedCallback.BindUFunction(this, "OutroTimelineComponentFinishedCallback");
		OutroTimelineComponent->AddInterpFloat(FloatCurve, TimelineFloat);
		OutroTimelineComponent->SetTimelineFinishedFunc(onTimelineFinishedCallback);
		const float Length = 18.0f;
		OutroTimelineComponent->SetPlayRate(1.0f / Length);
		OutroTimelineComponent->SetLooping(false);
		OutroTimelineComponent->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		OutroTimelineComponent->PlayFromStart();
	}
}

void APlayerCharacter::OutroTimelineComponentCallback(float val)
{
	TArray<AActor *> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);

	APostProcessVolume *PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);
	PostProcessVolume->Settings.VignetteIntensity = UKismetMathLibrary::MapRangeClamped(val, 0, 1, 1, 1000);

	if (IntroText)
	{
		int32 TextIndex = UKismetMathLibrary::MapRangeClamped(val, 0.0f, 1.0f, 0.0f, ((float)AllOutroText.Num()) - 0.001f);

		IntroText->SetText(FText::FromString(AllOutroText[TextIndex]));
	}
}


void APlayerCharacter::OutroTimelineComponentFinishedCallback()
{
}