// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBattleCharacter.h"
#include "SSTCharacterMovementComponent.h"
#include "FollowCameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "InputActionValue.h"

// Sets default values
ABaseBattleCharacter::ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<USSTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CharacterMovement Constants Setting
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
}

// Called when the game starts or when spawned
void ABaseBattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseBattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseBattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

