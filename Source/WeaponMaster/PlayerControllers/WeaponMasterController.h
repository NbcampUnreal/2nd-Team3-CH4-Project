#pragma once

#include "CoreMinimal.h"
#include "EOSPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "WeaponMasterController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class WEAPONMASTER_API AWeaponMasterController : public AEOSPlayerController
{
	GENERATED_BODY()
public:
	AWeaponMasterController();

	virtual void SetupInputComponent() override;

	UFUNCTION(Server, Reliable)
	void Server_SendChatMessage(const FText& Message);

	UFUNCTION(Client, Reliable)
	void SetCurrentCharacterAtGI(TSubclassOf<ACharacter> CurrentCharacterClass);

	// Battle IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Template Input Actions
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	
	/** Crouch/Drop Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchDropAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction;
	
	// Additional Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IdentityAction;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ChatAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MenuAction;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	bool bIsChatBound = false;
	
	UFUNCTION()
	void HandleChatMessage(const FText& Message);
	
	UFUNCTION()
	void Chat();

	UFUNCTION()
	bool IsChatWidgetValid() const;

	UFUNCTION()
	void Menu();
};
