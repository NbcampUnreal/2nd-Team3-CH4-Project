#include "KillLogWidget.h"

#include "KillLogEntryWidget.h"
#include "Components/ScrollBox.h"

void UKillLogWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UKillLogWidget::AddKillLogEntry(const FText& Killer, const FText& Victim)
{
	if (!KillLogEntryClass || !KillLogScrollBox) return;

	if (UKillLogEntryWidget* NewEntry = CreateWidget<UKillLogEntryWidget>(GetWorld(), KillLogEntryClass))
	{
		NewEntry->Setup(Killer, Victim);
		KillLogScrollBox->AddChild(NewEntry);
		KillLogScrollBox->ScrollToEnd();

		while (KillLogScrollBox->GetChildrenCount() > 5)
		{
			KillLogScrollBox->RemoveChildAt(0);
		}

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("RemoveKillLogEntry"), NewEntry);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.0f, false);
	}
}

void UKillLogWidget::RemoveKillLogEntry(UUserWidget* Entry)
{
	if (KillLogScrollBox && Entry)
	{
		KillLogScrollBox->RemoveChild(Entry);
	}
}
