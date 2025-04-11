// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SingleUI/UserSelectButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UUserSelectButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUserSelectButton::SetButtonText(const FText& NewText)
{
	if (ButtonTextBlock)
	{
		ButtonTextBlock->SetText(NewText);
	}
}