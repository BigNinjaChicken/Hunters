// Fill out your copyright notice in the Description page of Project Settings.


#include "AIConductor.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"
#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationInvokerComponent.h"

// Sets default values
AAIConductor::AAIConductor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIConductor::BeginPlay()
{
	Super::BeginPlay();
	
	// Spawn the AI controller and attach it to the AI character
    PassengerController = GetWorld()->SpawnActor<AMyAIController>(AMyAIController::StaticClass());
    if (PassengerController)
    {
        PassengerController->Possess(this);
    }
}

// Called every frame
void AAIConductor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
