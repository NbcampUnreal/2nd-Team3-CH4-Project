// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "Animation/AnimInstance.h"
#include "BaseBattleAnimInstance.generated.h"

class ABaseBattleCharacter;
class USSTCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UBaseBattleAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBaseBattleAnimInstance();

protected:
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	TObjectPtr<ABaseBattleCharacter> Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	TObjectPtr<USSTCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActiveEffects")
	TArray<EBehaviorEffect> ActiveBehaviorEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	bool bShouldMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	bool bIsDashing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	bool bIsCrouched;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	bool bIsWallSliding;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementData")
	bool bIsFacingRight;
};
