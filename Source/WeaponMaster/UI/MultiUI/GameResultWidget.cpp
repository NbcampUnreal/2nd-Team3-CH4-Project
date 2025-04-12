// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MultiUI/GameResultWidget.h"
#include "UI/MultiUI/ResultPlayerEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

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
        Data.Damage = FMath::RandRange(100, 9999);
        Data.Icon = nullptr;

        TestData.Add(Data);
    }
    // 결과 적용
    PopulatePlayerEntries(TestData);

    UE_LOG(LogTemp, Warning, TEXT("테스트 데이터 생성 완료 (%d명)"), TestData.Num());
}

//void UGameResultWidget::PopulatePlayerEntries(const TArray<FPlayerResultData>& ResultList)
//{
//    if (!PlayerEntryList || !ResultEntryClass)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("PlayerEntryList 또는 ResultEntryClass가 설정되지 않았습니다."));
//        return;
//    }
//
//    PlayerEntryList->ClearChildren(); // 기존 항목 제거
//    UE_LOG(LogTemp, Warning, TEXT("PlayerEntryList 초기화 완료"));
//
//    for (const FPlayerResultData& Entry : ResultList)
//    {
//        UResultPlayerEntryWidget* NewEntry = CreateWidget<UResultPlayerEntryWidget>(this, ResultEntryClass);
//        if (!NewEntry)
//        {
//            UE_LOG(LogTemp, Error, TEXT("위젯 생성 실패"));
//            continue;
//        }
//
//        UE_LOG(LogTemp, Warning, TEXT("위젯 생성 성공 - 닉네임: %s, 킬: %d, 데미지: %d, 골드: %d"),
//            *Entry.Nickname, Entry.Kills, Entry.Damage, Entry.Gold);
//
//        NewEntry->SetPlayerResultInfo(
//            Entry.Icon,
//            FText::FromString(Entry.Nickname),
//            Entry.Kills,
//            Entry.Damage,
//            Entry.Gold
//        );
//        PlayerEntryList->AddChild(NewEntry);
//    }
//    UE_LOG(LogTemp, Warning, TEXT("PopulatePlayerEntries 종료"));
//}

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
