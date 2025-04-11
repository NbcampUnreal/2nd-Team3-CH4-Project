// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultPlayerEntryWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class WEAPONMASTER_API UResultPlayerEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 점수 정보를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetPlayerResultInfo(UTexture2D* PlayerIcon, const FText& Nickname, int32 Kills, int32 Damage, int32 Gold);

protected:
	virtual void NativeConstruct() override;

	// 위젯 바인딩
	UPROPERTY(meta = (BindWidget))
	UImage* PlayerIconImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NicknameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;
};
