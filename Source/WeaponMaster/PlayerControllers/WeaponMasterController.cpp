// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMasterController.h"

#include "EnhancedInputSubsystems.h"

AWeaponMasterController::AWeaponMasterController()
{
	BattleMappingContext = nullptr;
	HorizontalInputAction = nullptr;
	VerticalInputAction = nullptr;
	WeakAttackAction = nullptr;
	StrongAttackAction = nullptr;
	SpecialAction = nullptr;
	DefenceAction = nullptr;
	DashAction = nullptr;
	PickingItemAction = nullptr;
	MenuOnOffAction = nullptr;
}

void AWeaponMasterController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(BattleMappingContext, 0);
	}
}

void AWeaponMasterController::HorizontalInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::HorizontalInput Binding Function"));
}

void AWeaponMasterController::VerticalInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::VerticalInput Binding Function"));
}

void AWeaponMasterController::WeakAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::WeakAttack Binding Function"));
}

void AWeaponMasterController::StrongAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::StrongAttack Binding Function"));
}

void AWeaponMasterController::SpecialAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::SpecialAttack Binding Function"));
}

void AWeaponMasterController::Defence(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::Defence Binding Function"));
}

void AWeaponMasterController::Dash(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::Dash Binding Function"));
}

void AWeaponMasterController::PickingItem(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::PickingItem Binding Function"));
}

void AWeaponMasterController::MenuOnOff(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AWeaponMasterController::MenuOnOff Binding Function"));
}
