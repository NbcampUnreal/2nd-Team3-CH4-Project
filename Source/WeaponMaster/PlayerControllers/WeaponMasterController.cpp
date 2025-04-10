// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponMasterController.h"
#include "EnhancedInputSubsystems.h"
#include "GameModes/BattleGMInterface.h"
#include "GameFramework/GameModeBase.h"


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
	ChatAction = nullptr;
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

	if (auto CastedGameInstance = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
	{
		TSubclassOf<ACharacter> CharacterClass = CastedGameInstance->CharacterClass;
		FName ItemName = CastedGameInstance->ItemName;
	
		ServerSetPlayerCharacter(CharacterClass, ItemName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AWeaponMasterController::BeginPlay : GameInstance Cast Failed."))
	}
}

void AWeaponMasterController::ServerSetPlayerCharacter_Implementation(TSubclassOf<ACharacter> CharacterClass, FName ItemName)
{
	if (const auto CastedGameMode = Cast<IBattleGMInterface>(GetWorld()->GetAuthGameMode()))
	{
		CastedGameMode->SetPlayerCharacter(CharacterClass, ItemName, this);
	}
}
