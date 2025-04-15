// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MultiUI/GameResultWidget.h"
#include "UI/MultiUI/ResultPlayerEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/SingleUI/UserSelectButton.h"
#include <Kismet/GameplayStatics.h>

void UGameResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 테스트용 데이터 생성
    TArray<FPlayerResultData> TestData;

    for (int32 i = 0; i < 6; ++i)
    {
        FPlayerResultData Data;
        Data.Nickname = FString::Printf(TEXT("Player_%d"), i + 1);
        Data.Kills = FMath::RandRange(0, 10);
        Data.Deaths = FMath::RandRange(0, 5);
        Data.Damage = FMath::RandRange(100, 9999);
        Data.Icon = nullptr;

        TestData.Add(Data);
    }

    if (MainButton)
    {
        MainButton->SetButtonText(FText::FromString(TEXT("메인으로")));
        MainButton->OnClicked.AddDynamic(this, &UGameResultWidget::OnMainButtonClicked);
    }
    // Kill순 정렬 후 결과 적용
    SortPlayerResultsByKill(TestData);
    PopulatePlayerEntries(TestData);

    UE_LOG(LogTemp, Warning, TEXT("테스트 데이터 생성 완료 (%d명)"), TestData.Num());
}

void UGameResultWidget::PopulatePlayerEntries(const TArray<FPlayerResultData>& ResultList)
{
    if (!PlayerEntryList || !ResultEntryClass)
    {
        return;
    }

    PlayerEntryList->ClearChildren(); // 기존 항목 제거

    for (const FPlayerResultData& Entry : ResultList)
    {
        UResultPlayerEntryWidget* NewEntry = CreateWidget<UResultPlayerEntryWidget>(this, ResultEntryClass);
        if (!NewEntry)
        {
            continue;
        }
        NewEntry->SetPlayerResultInfo(
            Entry.Icon,
            FText::FromString(Entry.Nickname),
            Entry.Kills,
            Entry.Deaths,
            Entry.Damage
        );

        UVerticalBoxSlot* NewSlot = PlayerEntryList->AddChildToVerticalBox(NewEntry);
        if (NewSlot)
        {
            NewSlot->SetPadding(FMargin(0.f, 8.f)); // 위아래 여백
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("PopulatePlayerEntries 종료"));
}

void UGameResultWidget::OnMainButtonClicked()
{

    FName MapID = TEXT("StartMap");

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

void UGameResultWidget::SortPlayerResultsByKill(TArray<FPlayerResultData>& ResultList)
{
    ResultList.Sort([](const FPlayerResultData& A, const FPlayerResultData& B)
        {
            return A.Kills > B.Kills; // 내림차순 (Kill이 높은 순)
        });
}
