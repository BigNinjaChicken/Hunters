#include "PassengerCharacter.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"
#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationInvokerComponent.h"

APassengerCharacter::APassengerCharacter()
{
   
}

void APassengerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn the AI controller and attach it to the AI character
    PassengerController = GetWorld()->SpawnActor<AMyAIController>(AMyAIController::StaticClass());
    if (PassengerController)
    {
        PassengerController->Possess(this);
    }
}

void APassengerCharacter::Tick(float DeltaSeconds)
{
	// If the AI character has no target location, generate a new one
	if (TargetLocation.IsZero())
	{
		FVector StartingLocation = GetActorLocation();
		UNavigationSystemV1 *NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (NavSys)
		{
			FNavLocation ResultLocation;
			bool RandLocation = NavSys->GetRandomReachablePointInRadius(StartingLocation, 500.0f, ResultLocation);
			TargetLocation = ResultLocation.Location;
		}
	}

	// Move towards the target location
	if (!TargetLocation.IsZero())
	{
		PassengerController->MoveToLocation(TargetLocation);
		FVector CurrentLocation = GetActorLocation();
		float NearDistance = 100.0f;
		if (sqrt((TargetLocation.X - CurrentLocation.X) * (TargetLocation.X - CurrentLocation.X) +
				 (TargetLocation.Y - CurrentLocation.Y) * (TargetLocation.Y - CurrentLocation.Y) +
				 (TargetLocation.Z - CurrentLocation.Z) * (TargetLocation.Z - CurrentLocation.Z)) < NearDistance)
		{
			TargetLocation = FVector::ZeroVector;
		}
	}
}
