// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/StatusTypes.h"
#include "DebuffIconWidget.h"
#include "DebuffWidget.generated.h"

class UHorizontalBox;
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

	virtual void NativeConstruct() override;
	
public:
	void UpdateWidget(const TArray<EBehaviorEffect>& ActiveBehaviorEffects);
};
