// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MultiUI/ResultPlayerEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UResultPlayerEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기화가 필요하면 여기서 처리
}

FString TruncateNicName(const FString& Name, int32 MaxLength)
{
	if (Name.Len() <= MaxLength)
	{
		return Name;
	}

	return Name.Left(MaxLength) + TEXT("...");
}

void UResultPlayerEntryWidget::SetPlayerResultInfo(UTexture2D* InIcon, const FText& InNickname, int32 InKills, int32 InDeaths, int32 InDamage)
{
	if (PlayerIconImage)
	{
		PlayerIconImage->SetBrushFromTexture(InIcon);
	}

	if (NicknameText)
	{
		FString PlayerNickname = TruncateNicName(InNickname.ToString(), 7);
		NicknameText->SetText(FText::FromString(PlayerNickname));
	}

	if (KillCountText)
	{
		if (InKills == 999)
		{
			FString KillString = FString::Printf(TEXT("토벌 실패"));
			KillCountText->SetText(FText::FromString(KillString));
		}
		else if (InKills == 1000)
		{
			FString KillString = FString::Printf(TEXT("토벌 성공"));
			KillCountText->SetText(FText::FromString(KillString));
		}
		else
		{
			FString KillString = FString::Printf(TEXT("%d Kill"), InKills);
			KillCountText->SetText(FText::FromString(KillString));
		}
	}

	if (DeathsCountText)
	{
		FString DeathsString = FString::Printf(TEXT("%d Deaths"), InDeaths);
		DeathsCountText->SetText(FText::FromString(DeathsString));
	}

	if (DamageText)
	{
		DamageText->SetText(FText::AsNumber(InDamage));
	}
}
