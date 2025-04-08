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
#include "Characters/Components/StateComponent/StateComponent.h"
#include "Characters/Components/StateComponent/States/CharacterInputState.h"
#include "WeaponMaster/PlayerControllers/WeaponMasterController.h"

// Sets default values
ABaseBattleCharacter::ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USSTCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// StateComponent
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));

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
	InputComponent = PlayerInputComponent;

	StateComponent->OnStateComponentReady.BindUObject(this, &ABaseBattleCharacter::BindInputFunctions);
}

void ABaseBattleCharacter::BindInputFunctions()
{
	AWeaponMasterController* WeaponMasterController = Cast<AWeaponMasterController>(GetController());

	UE_LOG(LogTemp, Display, TEXT("BindInputFunctions: Call"));

	if (!IsValid(WeaponMasterController))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::BindInputFunctions : Controller Casting Failed."));
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : EnhancedInputComponent Casting Failed."));
		return;
	}

	UCharacterInputState* CharacterInputState = StateComponent->GetInputState();

	// Jumping
	if (WeaponMasterController->JumpAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Triggered,
		                                   CharacterInputState, &UCharacterInputState::JumpOrDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Completed,
		                                   CharacterInputState, &UCharacterInputState::ReleaseJump);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->JumpAction"));
	}

	// Crouching/Dropping
	if (WeaponMasterController->CrouchDropAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->CrouchDropAction, ETriggerEvent::Triggered,
		                                   CharacterInputState, &UCharacterInputState::CrouchDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->CrouchDropAction, ETriggerEvent::Completed,
		                                   CharacterInputState, &UCharacterInputState::StopCrouchDrop);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->CrouchDropAction"));
	}

	// Moving
	if (WeaponMasterController->MoveAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->MoveAction, ETriggerEvent::Triggered,
		                                   CharacterInputState, &UCharacterInputState::Move);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->MoveAction"));
	}

	// Dashing
	if (WeaponMasterController->DashAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->DashAction, ETriggerEvent::Started,
		                                   CharacterInputState, &UCharacterInputState::Dash);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->DashAction"));
	}

	// WeakAttack
	if (WeaponMasterController->WeakAttackAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->WeakAttackAction, ETriggerEvent::Started,
		                                   CharacterInputState, &UCharacterInputState::WeakAttack);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// StrongAttack
	if (WeaponMasterController->StrongAttackAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->StrongAttackAction, ETriggerEvent::Started,
		                                   CharacterInputState, &UCharacterInputState::StrongAttack);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// Identity
	if (WeaponMasterController->IdentityAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->IdentityAction, ETriggerEvent::Started,
		                                   CharacterInputState, &UCharacterInputState::Identity);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// Defence
	if (WeaponMasterController->DefenceAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->DefenceAction, ETriggerEvent::Started,
		                                   CharacterInputState, &UCharacterInputState::Defence);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// PickingItem
	if (WeaponMasterController->PickingItemAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->PickingItemAction, ETriggerEvent::Started,
		                                   CharacterInputState, &UCharacterInputState::PickingItem);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// MenuOnOff
	if (WeaponMasterController->MenuOnOffAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->MenuOnOffAction, ETriggerEvent::Started,
		                                   CharacterInputState, &UCharacterInputState::MenuOnOff);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}
}

void ABaseBattleCharacter::WeakAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::WeakAttack !"));
}

void ABaseBattleCharacter::StrongAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::StrongAttack !"));
}

void ABaseBattleCharacter::Identity()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Identity !"));
}

void ABaseBattleCharacter::Defence()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Defence !"));
}

void ABaseBattleCharacter::PickingItem()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::PickingItem !"));
}

void ABaseBattleCharacter::MenuOnOff()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::MenuOnOff !"));
}
