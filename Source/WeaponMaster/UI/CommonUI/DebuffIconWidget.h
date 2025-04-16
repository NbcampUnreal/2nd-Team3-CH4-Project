// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebuffIconWidget.generated.h"

class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UDebuffIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> IconWrapper;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DebuffIcon;

};
