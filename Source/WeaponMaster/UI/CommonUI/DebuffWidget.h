// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/StatusTypes.h"

#include "DebuffWidget.generated.h"

class UDebuffIconWidget;
class UHorizontalBox;
class UTextBlock;

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UDebuffWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDebuffIconWidget> StunIconWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDebuffIconWidget> ConfusedIconWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDebuffIconWidget> SilenceIconWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> DebuffIconWrapper;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameBox;
	
	virtual void NativeConstruct() override;
	
public:
	void SetPlayerNameBox(const FString& NewPlayerName);
	void UpdateWidget(const TArray<EBehaviorEffect>& ActiveBehaviorEffects);
};
