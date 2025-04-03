#include "ChoiceWidget.h"
#include "Components/Button.h"
#include "Engine/Engine.h"

void UChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ButtonBack)
	{
		ButtonBack->OnClicked.AddDynamic(this, &UChoiceWidget::OnBackClicked);
	}
	if (ButtonNext)
	{
		ButtonNext->OnClicked.AddDynamic(this, &UChoiceWidget::OnNextClicked);
	}
	
}

void UChoiceWidget::OnNextClicked()
{
	if (CurrentWidgetState == EWidgetState::CharacterChoice)
	{
		CurrentWidgetState = EWidgetState::WeaponChoice;
		UpdateChoiceWidget();

	}
	else if (CurrentWidgetState == EWidgetState::WeaponChoice)
	{
		LogMessage("State END Go to Next Level");
	}
}

void UChoiceWidget::OnBackClicked()
{
	if (CurrentWidgetState == EWidgetState::CharacterChoice)
	{
		LogMessage("State END Go to Prev Level");

	}
	else if (CurrentWidgetState == EWidgetState::WeaponChoice)
	{
		CurrentWidgetState = EWidgetState::CharacterChoice;
		UpdateChoiceWidget();
	}
}
void UChoiceWidget::UpdateChoiceWidget()
{
	if (CurrentWidgetState == EWidgetState::CharacterChoice)
	{
		LogMessage("CharacterChoice");
	}
	else if(CurrentWidgetState == EWidgetState::CharacterChoice)
	{
		LogMessage("Weapon Choice");
	}
	else
	{
		LogMessage("???????????");
	}
	
}
void UChoiceWidget::LogMessage(const FString& Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
	}
}