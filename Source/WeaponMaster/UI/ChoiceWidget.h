// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChoiceWidget.generated.h"

class UButton;

/**
* 위젯 호출되는 순서 : 
* 캐릭터 선택 화면 -> 무기 선택 화면 
*/

UENUM(BlueprintType)
enum class EWidgetState : uint8
{
	CharacterChoice UMETA(DisplayName = "Character Choice"),
	WeaponChoice    UMETA(DisplayName = "Weapon Choice")
};


UCLASS()
class WEAPONMASTER_API UChoiceWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonBack;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonNext;

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnBackClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget State")
	EWidgetState CurrentWidgetState = EWidgetState::CharacterChoice;

	void UpdateChoiceWidget();

	void LogMessage(const FString& Message);
};
