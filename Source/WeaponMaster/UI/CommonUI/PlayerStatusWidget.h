// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;

UCLASS()
class WEAPONMASTER_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION()
    void SetPlayerName(const FString& Name);

    UFUNCTION()
    void SetThumbnail(UTexture2D* Thumbnail);

protected:
    // UI 구성요소
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PlayerNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> PlayerThumbnail;
};
