#include "PlayerNameWidget.h"

void UPlayerNameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PlayerNameText)
	{
		PlayerNameText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerNameWidget::SetPlayerName(const FString& PlayerName) const
{
	if (PlayerNameText)
	{
		FText PlayerNameT = FText::FromString(PlayerName);
		PlayerNameText->SetText(PlayerNameT);
	}
}

