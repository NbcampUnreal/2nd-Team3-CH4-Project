// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SingleUI/SingleStageSelectWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include <Kismet/GameplayStatics.h>

void USingleStageSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &USingleStageSelectWidget::OnConfirmClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USingleStageSelectWidget::OnBackButtonClicked);
	}
	
	CreateStageButtons();
}

void USingleStageSelectWidget::CreateStageButtons()
{
	// 맵 데이터 추가
	FStageInfo Map1;
	Map1.MapID = "SinglePlayMap";
	Map1.MapName = FText::FromString(TEXT("연습장"));
	Map1.MapDescription = FText::FromString(TEXT("기본 조작을 익히는 공간입니다."));
	Map1.MapThumbnail = LoadObject<UTexture2D>(nullptr, TEXT("/Game/WeaponMaster/Assets/Image/TestSingleMapImage.TestSingleMapImage"));

	FStageInfo Map2;
	Map2.MapID = "ICEMap";
	Map2.MapName = FText::FromString(TEXT("빙산맵"));
	Map2.MapDescription = FText::FromString(TEXT("추가 예정"));
	Map2.MapThumbnail = LoadObject<UTexture2D>(nullptr, TEXT("/Game/WeaponMaster/Assets/Image/ICEMAP_Image.ICEMAP_Image"));

	StageList = { Map1, Map2 };

	// 버튼 생성
	for (int32 i = 0; i < StageList.Num(); i++)
	{
		const FStageInfo& Info = StageList[i];
		if (!LeftPanel) continue;

		USingleStageButtonWidget* NewButton = CreateWidget<USingleStageButtonWidget>(this, StageButtonClass);
		NewButton->SetStageInfo(Info);
		NewButton->OnStageSelected.AddDynamic(this, &USingleStageSelectWidget::HandleStageSelected);
		LeftPanel->AddChild(NewButton);

		// 첫 번째 버튼을 자동으로 선택된 상태로 설정
		if (i == 0)
		{
			NewButton->SimulateClick(); // 강제 클릭
		}
	}
}

void USingleStageSelectWidget::HandleStageSelected(const FStageInfo& SelectedInfo)
{
	CurrentStageInfo = SelectedInfo;

	if (SelectedMapNameText)
		SelectedMapNameText->SetText(SelectedInfo.MapName);

	if (SelectedMapDescriptionText)
		SelectedMapDescriptionText->SetText(SelectedInfo.MapDescription);

	if (SelectedMapImage)
		SelectedMapImage->SetBrushFromTexture(SelectedInfo.MapThumbnail);
}

void USingleStageSelectWidget::OnConfirmClicked()
{
	FName MapID = TEXT("SinglePlayMap");

	if (!MapID.IsNone())
	{
		RemoveFromParent();
		// 해당 맵으로 이동
		UGameplayStatics::OpenLevel(this, MapID);
	}
	/*if (!CurrentStageInfo.MapID.IsNone())
	{
		UGameplayStatics::OpenLevel(this, CurrentStageInfo.MapID);
	}*/
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Map is Null!"));
	}
}

void USingleStageSelectWidget::OnBackButtonClicked()
{
	RemoveFromParent();  // 현재 위젯 제거

	if (WBP_SingleSelectionClass)
	{
		UUserWidget* SelectionWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_SingleSelectionClass);
		if (SelectionWidget)
		{
			SelectionWidget->AddToViewport();
		}
	}
}

