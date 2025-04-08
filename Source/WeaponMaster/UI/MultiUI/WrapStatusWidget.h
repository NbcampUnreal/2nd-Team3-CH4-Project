// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WrapStatusWidget.generated.h"

class UHorizontalBox;
class UPlayerStatusWidget;

UCLASS()
class WEAPONMASTER_API UWrapStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializePlayerStatus();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> PlayerContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float WidgetSpacing = 10.0f;
};
