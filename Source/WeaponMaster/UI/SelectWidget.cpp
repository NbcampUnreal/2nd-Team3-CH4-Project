#include "SelectWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "Input/Reply.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void USelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeWidget();
}
void USelectWidget::SetWidgetState(EWidgetState NewState)
{
	if (NewState == EWidgetState::CharacterChoice)
	{
		CharacterImage->SetVisibility(ESlateVisibility::Visible);
		WeaponImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (NewState == EWidgetState::WeaponChoice)
	{
		CharacterImage->SetVisibility(ESlateVisibility::Hidden);
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
	}
}
void USelectWidget::InitializeWidget()
{
	CharacterImage->SetBrushFromTexture(CharacterDefaultTexture);
	WeaponImage->SetBrushFromTexture(WeaponDefaultTexture);
	HighlightImage->SetBrushFromTexture(HighlightTexture);
	CharacterImage->SetVisibility(ESlateVisibility::Visible);
	WeaponImage->SetVisibility(ESlateVisibility::Hidden);
	HighlightImage->SetVisibility(ESlateVisibility::Hidden);
}

void USelectWidget::UpdateHighlight(bool bIsHighlighted)
{
	//하이라이트를 업데이트할 때 상위 위젯에서 인덱스를 통해 하이라이트를 시켜줌
	if (bIsHighlighted)
	{
		HighlightImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HighlightImage->SetVisibility(ESlateVisibility::Hidden);
	}
}
