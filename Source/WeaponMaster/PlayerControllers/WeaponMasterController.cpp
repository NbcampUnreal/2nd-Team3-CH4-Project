// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponMasterController.h"
#include "EnhancedInputSubsystems.h"

AWeaponMasterController::AWeaponMasterController()
{
	DefaultMappingContext = nullptr;
	MoveAction = nullptr;
	CrouchDropAction= nullptr;
	WeakAttackAction = nullptr;
	StrongAttackAction = nullptr;
	IdentityAction = nullptr;
	DefenceAction = nullptr;
	DashAction = nullptr;
	PickingItemAction = nullptr;
	MenuOnOffAction = nullptr;
}

void AWeaponMasterController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("Player Pointer is null"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(Subsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("Player Subsystem is null"));
		return;
	}
	
	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is null"));
		return;
	}
	
	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

