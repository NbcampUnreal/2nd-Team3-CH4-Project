#include "ChoiceWidget.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

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
		PlaySound(SelectSound);
		CurrentWidgetState = EWidgetState::WeaponChoice;
		UpdateChoiceWidget();

	}
	else if (CurrentWidgetState == EWidgetState::WeaponChoice)
	{
		PlaySound(SelectSound);
		LogMessage("State END Go to Next Level");
		NextButtonClicked.Broadcast();
	}
}

void UChoiceWidget::OnBackClicked()
{
	if (CurrentWidgetState == EWidgetState::CharacterChoice)
	{
		PlaySound(SelectSound);
		LogMessage("State END Go to Prev Level");
		PrevButtonClicked.Broadcast();

	}
	else if (CurrentWidgetState == EWidgetState::WeaponChoice)
	{
		PlaySound(SelectSound);
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
}
void UChoiceWidget::LogMessage(const FString& Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
	}
}
void UChoiceWidget::PlaySound(TObjectPtr<USoundBase> Sound)
{
	if (!ensure(Sound))
	{
		return;
	}
	UGameplayStatics::PlaySound2D(this, Sound);
}
