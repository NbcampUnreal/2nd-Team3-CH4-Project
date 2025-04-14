// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MultiUI/ResultPlayerEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UResultPlayerEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기화가 필요하면 여기서 처리
}

void UResultPlayerEntryWidget::SetPlayerResultInfo(UTexture2D* InIcon, const FText& InNickname, int32 InKills, int32 InDeaths, int32 InDamage)
{
	if (PlayerIconImage)
	{
		PlayerIconImage->SetBrushFromTexture(InIcon);
	}

	if (NicknameText)
	{
		NicknameText->SetText(InNickname);
	}

	if (KillCountText)
	{
		FString KillString = FString::Printf(TEXT("%d Kill"), InKills);
		KillCountText->SetText(FText::FromString(KillString));
	}

	if (DamageText)
	{
		DamageText->SetText(FText::AsNumber(InDamage));
	}
}

