#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCooperateButtonClickedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathMatchButtonClickedDelegate);

UCLASS()
class WEAPONMASTER_API USessionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable, Category="SessionWidget|Events")
	FOnCooperateButtonClickedDelegate OnCooperateButtonClickedDelegate;

	UPROPERTY(BlueprintAssignable, Category="SessionWidget|Events")
	FOnDeathMatchButtonClickedDelegate OnDeathMatchButtonClickedDelegate;

	UFUNCTION()
	void SetTotalPlayers(int32 TotalPlayers) const;

	UFUNCTION()
	void SetTimer(int32 TimeRemain) const;

	UFUNCTION()
	void SetCooperateMapSelectedPlayers(int32 TotalPlayers) const;

	UFUNCTION()
	void SetDeathMatchMapSelectedPlayers(int32 TotalPlayers) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalPlayerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooperateText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DeathMatchText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CooperateButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeathMatchButton;

	UFUNCTION()
	void OnCooperateButtonClicked();

	UFUNCTION()
	void OnDeathMatchButtonClicked();
};
