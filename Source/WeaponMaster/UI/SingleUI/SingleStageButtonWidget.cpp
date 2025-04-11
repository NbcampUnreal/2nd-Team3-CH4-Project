// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SingleUI/SingleStageButtonWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void USingleStageButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &USingleStageButtonWidget::HandleClicked);
	}
}

void USingleStageButtonWidget::SetStageInfo(const FStageInfo& Info)
{
	CurrentStageInfo = Info;

	if (MapImage)
	{
		MapImage->SetBrushFromTexture(Info.MapThumbnail);
	}

	if (MapNameText)
	{
		MapNameText->SetText(Info.MapName);
	}
}

void USingleStageButtonWidget::HandleClicked()
{
	OnStageSelected.Broadcast(CurrentStageInfo);
}

void USingleStageButtonWidget::SimulateClick()
{
	HandleClicked(); // 실제 클릭 처리 함수 직접 호출
}
