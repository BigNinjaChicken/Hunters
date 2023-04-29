#include "PassengerCharacter.h"
#include "NavigationSystem.h"
#include "MyAIController.h"

APassengerCharacter::APassengerCharacter()
{
}

void APassengerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PassengerController = GetWorld()->SpawnActor<AMyAIController>(AMyAIController::StaticClass());
	if (PassengerController)
	{
		PassengerController->Possess(this);
	}
}

void APassengerCharacter::Tick(float DeltaSeconds)
{
	MoveToLocation(DeltaSeconds);
}

FVector APassengerCharacter::FindRandomTargetLocation()
{
	FVector StartingLocation = GetActorLocation();
	FNavLocation ResultLocation;
	if (UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(StartingLocation, 500.0f, ResultLocation))
	{
		return ResultLocation.Location;
	}
	
	// Add a default return value here
	return FVector::ZeroVector;
	UE_LOG(LogTemp, Warning, TEXT("GetRandomReachablePointInRadius() Failed"));

}


void APassengerCharacter::MoveToLocation(float DeltaSeconds)
{
	if (bIsWaiting)
	{
		return;
	}

	bIsWaiting = true;

	GetWorldTimerManager().SetTimer(
		WaitTimerHandle, [this]()
		{ bIsWaiting = false; },
		5.0f, false);

	PassengerController->MoveToLocation(FindRandomTargetLocation(), 50.0f);
}
