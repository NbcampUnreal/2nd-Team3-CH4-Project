// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponMaster/UI/SingleUI/SingleStageButtonWidget.h"
#include "SingleStageSelectWidget.generated.h"

class UVerticalBox;
class UHorizontalBox;
class UTextBlock;
class UImage;
class UButton;


UCLASS()
class WEAPONMASTER_API USingleStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* LeftPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SelectedMapNameText;

	UPROPERTY(meta = (BindWidget))
	UImage* SelectedMapImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SelectedMapDescriptionText;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	UFUNCTION()
	void OnBackButtonClicked();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapInfo", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USingleStageButtonWidget> StageButtonClass;

private:
	UFUNCTION()
	void OnConfirmClicked();

	UPROPERTY()
	TArray<FStageInfo> StageList;

	// 현재 선택된 맵
	UPROPERTY()
	FStageInfo CurrentStageInfo;

	// 버튼 생성 함수
	void CreateStageButtons();

	// 버튼 클릭 시 선택 처리
	UFUNCTION()
	void HandleStageSelected(const FStageInfo& SelectedInfo);
};