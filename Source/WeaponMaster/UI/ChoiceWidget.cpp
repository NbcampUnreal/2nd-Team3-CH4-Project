#include "ChoiceWidget.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "SelectWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "GameFramework/PlayerController.h"

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
	if (!SelectWidgetGridPanel || !SelectWidgetClass)
	{
		return;
	}
	SelectWidgetGridPanel->ClearChildren();
	SelectWidgets.Empty();

	for (int32 Index = 0; Index < FixedSlotCount; Index++)
	{
		USelectWidget* NewWidget = CreateWidget<USelectWidget>(GetWorld(), SelectWidgetClass);
		if (NewWidget)
		{
			SelectWidgets.Add(NewWidget);
			UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(SelectWidgetGridPanel->AddChild(NewWidget));
			if (GridSlot)
			{
				GridSlot->SetColumn(Index % Columns);
				GridSlot->SetRow(Index / Columns);
			}
		}
	}
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget());
		PC->SetInputMode(InputMode);
	}
	HighlightSelectWidget();
}

void UChoiceWidget::OnNextClicked()
{
	if (!SelectWidgets.IsEmpty())
	{
		if (CurrentState == EWidgetState::CharacterChoice)
		{
			UnHighlightSelectWidget();
			PlaySound(SelectSound);
			ChangeSelectWidgetState(EWidgetState::WeaponChoice);
			CurrentState = EWidgetState::WeaponChoice;
			WidgetIndex = 0;
			HighlightSelectWidget();
		}
		else
		{
			PlaySound(SelectSound);
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this]()
				{
					NextButtonClicked.Broadcast();
				});
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
			}
			//NextButtonClicked.Broadcast();
		}
	}
}

void UChoiceWidget::OnBackClicked()
{
	if (!SelectWidgets.IsEmpty())
	{
		if (CurrentState == EWidgetState::WeaponChoice)
		{
			UnHighlightSelectWidget();
			PlaySound(SelectSound);
			CurrentState = EWidgetState::CharacterChoice;
			ChangeSelectWidgetState(EWidgetState::CharacterChoice);
			WidgetIndex = 0;
			HighlightSelectWidget();
		}
		else
		{
			PlaySound(SelectSound);
			PrevButtonClicked.Broadcast();
		}
	}
}
//2 .그다음에 DA나 DT 활용해서 이미지파일 집어넣자 

void UChoiceWidget::PlaySound(TObjectPtr<USoundBase> Sound)
{
	if (!ensure(Sound))
	{
		return;
	}
	UGameplayStatics::PlaySound2D(this, Sound);
}

void UChoiceWidget::HighlightSelectWidget()
{
	SelectWidgets[WidgetIndex]->UpdateHighlight(true);
}

void UChoiceWidget::UnHighlightSelectWidget()
{
	SelectWidgets[WidgetIndex]->UpdateHighlight(false);
}

void UChoiceWidget::ChangeSelectWidgetState(EWidgetState ChangedState)
{
	for (USelectWidget* Temp : SelectWidgets)
	{
		Temp->SetWidgetState(ChangedState);
	}
}

TArray<TObjectPtr<USelectWidget>> UChoiceWidget::GetSelectWidgets()
{
	return SelectWidgets;
}

FReply UChoiceWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();
	bool bIsHandled = false;
	if (Key == EKeys::Left)
	{
		UnHighlightSelectWidget();
		WidgetIndex = (WidgetIndex - 1 + SelectWidgets.Num()) % SelectWidgets.Num();
		if (MoveSound)
		{
			PlaySound(MoveSound);
		}
		HighlightSelectWidget();
		bIsHandled = true;
	}
	else if (Key == EKeys::Right)
	{
		UnHighlightSelectWidget();
		WidgetIndex = (WidgetIndex + 1) % SelectWidgets.Num();

		if (MoveSound)
		{
			PlaySound(MoveSound);
		}
		HighlightSelectWidget();
		bIsHandled = true;
	}
	else if (Key == EKeys::Enter)
	{
		if (SelectSound)
		{
			PlaySound(SelectSound);
		}
		OnNextClicked();
	}
	else if (Key == EKeys::BackSpace)
	{
		if (SelectSound)
		{
			PlaySound(SelectSound);
		}
		OnBackClicked();
	}
	if (bIsHandled)
	{
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}