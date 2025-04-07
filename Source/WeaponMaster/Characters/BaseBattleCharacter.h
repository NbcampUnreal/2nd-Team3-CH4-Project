// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISSTCharacterBindingFunctions.h"
#include "SSTCharacter.h"
#include "BaseBattleCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class WEAPONMASTER_API ABaseBattleCharacter : public ASSTCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move1(const FInputActionValue& Value);
};
