// Fill out your copyright notice in the Description page of Project Settings.


#include "DebuffWidget.h"
#include "Components/HorizontalBox.h"

void UDebuffWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	StunIconWidget->SetVisibility(ESlateVisibility::Collapsed);
	ConfusedIconWidget->SetVisibility(ESlateVisibility::Collapsed);
	SilenceIconWidget->SetVisibility(ESlateVisibility::Collapsed);
	UE_LOG(LogTemp, Display, TEXT("UDebuffWidget::NativeConstruct : Created."));
}

void UDebuffWidget::UpdateWidget(const TArray<EBehaviorEffect>& ActiveBehaviorEffects)
{
	UE_LOG(LogTemp, Display, TEXT("UDebuffWidget::UpdateWidget : Called."));
	if (ActiveBehaviorEffects.Contains(EBehaviorEffect::Stun))
	{
		StunIconWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		StunIconWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (ActiveBehaviorEffects.Contains(EBehaviorEffect::Confused))
	{
		ConfusedIconWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ConfusedIconWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (ActiveBehaviorEffects.Contains(EBehaviorEffect::Silence))
	{
		SilenceIconWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SilenceIconWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
