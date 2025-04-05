// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetStateData.h"
#include "ChoiceWidget.generated.h"

class UButton;
class USelectWidget;
class UUniformGridPanel;
/**
* 위젯 호출되는 순서 : 
* 캐릭터 선택 화면 -> 무기 선택 화면 
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChoiceAction);



UCLASS()
class WEAPONMASTER_API UChoiceWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	FOnChoiceAction NextButtonClicked;
	FOnChoiceAction PrevButtonClicked;
	TArray<TObjectPtr<USelectWidget>> GetSelectWidgets();
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonBack;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonNext;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> SelectWidgetGridPanel;

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnBackClicked();

	void LogMessage(const FString& Message);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> MoveSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> SelectSound;

	void PlaySound(TObjectPtr<USoundBase> Sound);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<USelectWidget> SelectWidgetClass;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	int32 FixedSlotCount = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	int32 Columns = 4;

	TArray<TObjectPtr<USelectWidget>> SelectWidgets;

	EWidgetState CurrentState = EWidgetState::CharacterChoice;

	int16 WidgetIndex = 0;

	void HighlightSelectWidget();
	void UnHighlightSelectWidget();

	

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
