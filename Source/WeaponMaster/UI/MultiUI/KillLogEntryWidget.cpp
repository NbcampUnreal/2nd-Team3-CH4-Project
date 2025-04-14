#include "KillLogEntryWidget.h"

#include "Components/TextBlock.h"

void UKillLogEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(AutoRemoveTimer, this, &UKillLogEntryWidget::HandleAutoRemove, 5.0f, false);
}

void UKillLogEntryWidget::Setup(const FText& KillerName, const FText& VictimName) const
{
	if (KillerText)
	{
		KillerText->SetText(KillerName);
	}
	if (VictimText)
	{
		VictimText->SetText(VictimName);
	}
}

void UKillLogEntryWidget::HandleAutoRemove()
{
	RemoveFromParent();
}
