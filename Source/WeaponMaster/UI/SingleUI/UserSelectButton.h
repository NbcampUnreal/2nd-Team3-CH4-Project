// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserSelectButton.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class WEAPONMASTER_API UUserSelectButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& NewText);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* MainButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ButtonTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundImage;
};