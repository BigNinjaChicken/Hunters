// Fill out your copyright notice in the Description page of Project Settings.


#include "EmemyCharacter.h"

// Sets default values
AEmemyCharacter::AEmemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEmemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEmemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEmemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

