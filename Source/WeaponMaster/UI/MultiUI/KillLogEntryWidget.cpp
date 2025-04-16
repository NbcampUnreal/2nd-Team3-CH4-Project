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
	UE_LOG(LogTemp, Warning, TEXT("KillLog Setup called"));
	UE_LOG(LogTemp, Warning, TEXT("DefaultKillIcon is %s"), DefaultKillIcon ? *DefaultKillIcon->GetName() : TEXT("NULL"));

	if (KillerText)
	{
		FString TruncatedKiller = TruncateName(KillerName.ToString(), 7);
		KillerText->SetText(FText::FromString(TruncatedKiller));
	}
	if (VictimText)
	{
		FString TruncatedVictim = TruncateName(VictimName.ToString(), 10);
		VictimText->SetText(FText::FromString(TruncatedVictim));
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
