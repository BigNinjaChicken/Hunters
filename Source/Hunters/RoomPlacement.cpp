// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomPlacement.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/PostProcessVolume.h"
#include "PlayerCharacter.h"

// Sets default values
ARoomPlacement::ARoomPlacement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent *RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	RoomEntrenceBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomEntrenceBoxComponent"));
	RoomEntrenceBoxComponent->SetupAttachment(RootComponent);
	RoomEntrenceBoxComponent->SetGenerateOverlapEvents(true);
	RoomEntrenceBoxComponent->SetBoxExtent(FVector(350.0f, 350.0f, 350.0f));
}

// Called when the game starts or when spawned
void ARoomPlacement::BeginPlay()
{
	Super::BeginPlay();

	RoomEntrenceBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARoomPlacement::OnOverlapBegin);
	RoomEntrenceBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ARoomPlacement::OnOverlapEnd);
}

void ARoomPlacement::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (!bIsLastRoom) {
		return;
	}


}

void ARoomPlacement::OnOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex) {
	if (!bIsLastRoom) {
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        // OtherActor is a PlayerCharacter, do something with it
        PlayerCharacter->EndDemo();
    }

}

// Called every frame
void ARoomPlacement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoomPlacement::CreateBossRoom(FVector RoomEndOffset)
{

}