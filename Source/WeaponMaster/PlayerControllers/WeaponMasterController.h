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

	// Battle IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	// Template Input Actions
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;
	
	/** Crouch/Drop Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchDropAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DashAction;
	
	// Additional Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SpecialAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DefenceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> WeakAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> StrongAttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PickingItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MenuOnOffAction;

protected:
	virtual void BeginPlay() override;

	// // Template Input Binding Functions
	// /** Called for movement input */
	// void Move(const struct FInputActionValue& Value);
	//
	// /** Called for crouch/drop input */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	// void CrouchDrop();
	//
	// /** Called when releasing crouch/drop input */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	// void StopCrouchDrop();
	//
	// /** Called when jump pressed, which could also be a drop-down command */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	// void JumpOrDrop();
	//
	// /** Called for dash input */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	// void Dash();
	//
	// Additional Input Binding Functions
	void WeakAttack(const FInputActionValue& Value);
	void StrongAttack(const FInputActionValue& Value);
	void SpecialAttack(const FInputActionValue& Value);
	void Defence(const FInputActionValue& Value);
	void PickingItem(const FInputActionValue& Value);
	void MenuOnOff(const FInputActionValue& Value);
};
