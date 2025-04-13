// Copyright 2024 NFB Makes Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISSTInputBindFunctions.h"
#include "GameFramework/Character.h"
#include "SSTCharacter.generated.h"

/**
 * Extends the default ACharacter to provide:
 * - A custom character movement component, supporting dash, wallslide, etc.
 * - A FollowCamera component, which follows the character and can be modified via CameraTrigger objects.
 * - Input bindings for left/right motion, Jump, Dash, Crouch, and platform-drop
 * 
 * USSTCharacterMovementComponent should remain the sole authority on movement for this character. 
 */
UCLASS(config=Game)
class SST_API ASSTCharacter : public ACharacter, public ISSTInputBindFunctions
{
	GENERATED_BODY()

protected:
	/** Custom CharacterMovementComponent */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USSTCharacterMovementComponent> SSTCharacterMovementComponent;

	/** Camera Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UFollowCameraComponent> FollowCamera;

	/** Time in seconds after dropping through a platform before the button becomes usable for jump input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float DropThroughPlatformJumpLockout = .2f;
	
	bool IsJumpState = false;

public:
	ASSTCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	/** Called for movement input */
	virtual void Move(const struct FInputActionValue& Value) override;
	
	/** Called for crouch/drop input */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	virtual void CrouchDrop() override;
	
	/** Called when releasing crouch/drop input */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
 	virtual void StopCrouchDrop() override;
	
	/** Called when jump pressed, which could also be a drop-down command */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	virtual void JumpOrDrop() override;

	/** Called when releasing the jump button */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Movement")
	virtual void ReleaseJump() override;
	
	/** Called for dash input */
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	virtual void Dash() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE class USSTCharacterMovementComponent* GetSSTCharacterMovement() const { return SSTCharacterMovementComponent; }

	/** Can override in blueprint for custom dash checking on this character */
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Movement")
	bool CanDash() const;

	

	/* Overrides to work with custom movement modes */
	virtual bool CanCrouch() const override; 

	virtual void StopJumping() override;

	virtual void ResetJumpState() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void CheckJumpInput(float DeltaTime) override;

	/** Helper -- get collision query params to ignore this actor and its children*/
	virtual FCollisionQueryParams GetIgnoreSelfParams() const;
};

