// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WeaponMasterController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API AWeaponMasterController : public APlayerController
{
	GENERATED_BODY()
public:
	AWeaponMasterController();

	// Battle IMC, IA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> BattleMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* HorizontalInputAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* VerticalInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* WeakAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* StrongAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SpecialAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DefenceAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PickingItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MenuOnOffAction;

protected:
	virtual void BeginPlay() override;

	// Input Binding Functions
	void HorizontalInput(const FInputActionValue& Value);
	void VerticalInput(const FInputActionValue& Value);
	void WeakAttack(const FInputActionValue& Value);
	void StrongAttack(const FInputActionValue& Value);
	void SpecialAttack(const FInputActionValue& Value);
	void Defence(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void PickingItem(const FInputActionValue& Value);
	void MenuOnOff(const FInputActionValue& Value);
};
