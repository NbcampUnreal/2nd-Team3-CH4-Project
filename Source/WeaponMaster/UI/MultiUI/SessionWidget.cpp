#include "SessionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DeathMatchButton)
	{
		DeathMatchButton->OnClicked.AddDynamic(this, &USessionWidget::OnDeathMatchButtonClicked);
	}

	if (DeathMatchButton)
	{
		CooperateButton->OnClicked.AddDynamic(this, &USessionWidget::OnCooperateButtonClicked);
	}
}

void USessionWidget::SetTotalPlayers(const int32 TotalPlayers) const
{
	TotalPlayerText->SetText( FText::Format(
		NSLOCTEXT("SessionWidget", "PlayerCount", "{0} / 50"),
		FText::AsNumber(TotalPlayers)
	));
}

void USessionWidget::SetTimer(int32 TimeRemain) const
{
	RemainTimeText->SetText( FText::Format(
		NSLOCTEXT("SessionWidget", "PlayerCount", "{0}"),
		FText::AsNumber(TimeRemain)
	));
}

void USessionWidget::SetCooperateMapSelectedPlayers(const int32 TotalPlayers) const
{
	CooperateText->SetText( FText::Format(
		NSLOCTEXT("SessionWidget", "PlayerCount", "{0} / 50"),
		FText::AsNumber(TotalPlayers)
	));
}

void USessionWidget::SetDeathMatchMapSelectedPlayers(const int32 TotalPlayers) const
{
	DeathMatchText->SetText( FText::Format(
		NSLOCTEXT("SessionWidget", "PlayerCount", "{0} / 50"),
		FText::AsNumber(TotalPlayers)
	));
}

void USessionWidget::OnCooperateButtonClicked()
{
	OnCooperateButtonClickedDelegate.Broadcast();
}

void USessionWidget::OnDeathMatchButtonClicked()
{
	OnDeathMatchButtonClickedDelegate.Broadcast();
}