#include "KillLogEntryWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

void UKillLogEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(AutoRemoveTimer, this, &UKillLogEntryWidget::HandleAutoRemove, 5.0f, false);
}

FString TruncateName(const FString& Name, int32 MaxLength)
{
	if (Name.Len() <= MaxLength)
	{
		return Name;
	}

	return Name.Left(MaxLength) + TEXT("...");
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

	if (SkullIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkullIcon is VALID"));
		if (DefaultKillIcon)
		{
			SkullIcon->SetBrushFromTexture(DefaultKillIcon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SkullIcon is NULL"));
	}
}

void UKillLogEntryWidget::HandleAutoRemove()
{
	RemoveFromParent();
}
