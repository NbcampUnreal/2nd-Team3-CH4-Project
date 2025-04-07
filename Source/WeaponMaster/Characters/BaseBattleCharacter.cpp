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
#include "WeaponMaster/PlayerControllers/WeaponMasterController.h"

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
	AWeaponMasterController* WeaponMasterController = Cast<AWeaponMasterController>(GetController());

	UE_LOG(LogTemp, Display, TEXT("SetupPlayerInputComponent: Call"));
	
	if (!WeaponMasterController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::SetupPlayerInputComponent : Controller Casting Failed."))
		return;
	}
		
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::SetupPlayerInputComponent : EnhancedInputComponent Casting Failed."))
		return;
	}
	
	//Jumping
	if (WeaponMasterController->JumpAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Triggered, Cast<ASSTCharacter>(this), &ASSTCharacter::JumpOrDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Completed, this, &ASSTCharacter::ReleaseJump);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::SetupPlayerInputComponent : No WeaponMasterController->JumpAction"))
	}

	//Crouching/Dropping
	if (WeaponMasterController->CrouchDropAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->CrouchDropAction, ETriggerEvent::Triggered, this, &ASSTCharacter::CrouchDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->CrouchDropAction, ETriggerEvent::Completed, this, &ASSTCharacter::StopCrouchDrop);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::SetupPlayerInputComponent : No WeaponMasterController->JumpAction"))
	}
	
	//Moving
	if (WeaponMasterController->MoveAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->MoveAction, ETriggerEvent::Triggered, this, &ABaseBattleCharacter::Move1);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::SetupPlayerInputComponent : No WeaponMasterController->JumpAction"))
	}

	//Dashing
	if (WeaponMasterController->DashAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->DashAction, ETriggerEvent::Started, this, &ASSTCharacter::Dash);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::SetupPlayerInputComponent : No WeaponMasterController->JumpAction"))
	}
}

void ABaseBattleCharacter::Move1(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Move"));
}

