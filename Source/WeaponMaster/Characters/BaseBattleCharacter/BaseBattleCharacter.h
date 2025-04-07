// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IBaseBattleInputBindFunctions.h"
#include "SSTCharacter.h"
#include "BaseBattleCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class WEAPONMASTER_API ABaseBattleCharacter : public ASSTCharacter, public IBaseBattleInputBindFunctions
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Additional Input Binding Functions
	virtual void WeakAttack(const FInputActionValue& Value) override;
	virtual void StrongAttack(const FInputActionValue& Value) override;
	virtual void Identity(const FInputActionValue& Value) override;
	virtual void Defence(const FInputActionValue& Value) override;
	virtual void PickingItem(const FInputActionValue& Value) override;
	virtual void MenuOnOff(const FInputActionValue& Value) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};
