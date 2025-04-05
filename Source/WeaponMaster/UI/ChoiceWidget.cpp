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
	FString test1 = FString::Printf(TEXT("%d"), static_cast<int32>(CurrentState));
	LogMessage(test1);
	if (!SelectWidgets.IsEmpty())
	{
		if (CurrentState == EWidgetState::CharacterChoice)
		{
			UnHighlightSelectWidget();
			PlaySound(SelectSound);
			SelectWidgets[WidgetIndex]->SetWidgetState(EWidgetState::WeaponChoice);
			CurrentState = EWidgetState::WeaponChoice;
			WidgetIndex = 0;
			HighlightSelectWidget();
		}
		else
		{
			LogMessage("State END Go to Next Level");
			NextButtonClicked.Broadcast();
		}
	}
	LogMessage("????????????????");
	FString test = FString::Printf(TEXT("%d"), static_cast<int32>(CurrentState));
	LogMessage(test);

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
			SelectWidgets[WidgetIndex]->SetWidgetState(EWidgetState::CharacterChoice);
			WidgetIndex = 0;
			HighlightSelectWidget();
		}
		else
		{
			PlaySound(SelectSound);
			LogMessage("State END Go to Prev Level");
			PrevButtonClicked.Broadcast();
		}
	}
}
//2 .그다음에 DA나 DT 활용해서 이미지파일 집어넣자 
//버그 수정 및 이미지파일 변경해놓고 유아틱 이미지파일 제거하기 
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

void UChoiceWidget::HighlightSelectWidget()
{
	SelectWidgets[WidgetIndex]->UpdateHighlight(true);
}

void UChoiceWidget::UnHighlightSelectWidget()
{
	SelectWidgets[WidgetIndex]->UpdateHighlight(false);
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
		LogMessage("Left");
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