// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiGameHUD.h"
#include "WrapStatusWidget.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "GameFramework/PlayerState.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"


void AMultiGameHUD::BeginPlay()
{
	Super::BeginPlay();
   /* if (WrapStatusWidgetClass)
    {
        CreatePlayerWidgets();
    }*/
    LogMessage("BeginPlay HUD");
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMultiGameHUD::CreatePlayerWidgets, 5.0f, false);
	//CreatePlayerWidgets();
}

void AMultiGameHUD::TestDummyModule()
{
    if (!WrapStatusWidgetClass) return;

    
    WrapStatusWidget = CreateWidget<UWrapStatusWidget>(GetWorld(), WrapStatusWidgetClass);
    if (WrapStatusWidget)
    {
        WrapStatusWidget->InitializeDummyPlayerStatus(TestInt,1);
        WrapStatusWidget->AddToViewport();
    }
}

void AMultiGameHUD::TestChatModule(FString TestString,int32 TargetCharacterID)
{
    LogMessage("TestChatModule Start");
    if (!WrapStatusWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("WrapStatusWidget 인스턴스가 존재하지 않습니다."));
        return;
    }

    TArray<UWidget*> LeftWidgets = WrapStatusWidget->GetLeftTeamContainer()->GetAllChildren();
    if (LeftWidgets.IsEmpty())
    {
        LogMessage("IsEmpty");
    }
    for (UWidget* Widget : LeftWidgets)
    {
        LogMessage("Update Start");
        if (UPlayerStatusWidget* PSWidget = Cast<UPlayerStatusWidget>(Widget))
        {
            if (PSWidget->GetCharacterID() == TargetCharacterID)
            {
                PSWidget->UpdateChat(TestString);
                UE_LOG(LogTemp, Warning, TEXT("CharacterID %d의 PlayerStatusWidget에서 채팅 업데이트됨"), TargetCharacterID);
                LogMessage(" Left Update");
                return;
            }
        }

    }

    TArray<UWidget*> RightWidgets = WrapStatusWidget->GetRightTeamContainer()->GetAllChildren();
    for (UWidget* Widget : RightWidgets)
    {
        LogMessage("Update Start");
        if (UPlayerStatusWidget* PSWidget = Cast<UPlayerStatusWidget>(Widget))
        {
            if (PSWidget->GetCharacterID() == TargetCharacterID)
            {
                PSWidget->UpdateChat(TestString);
                LogMessage("Right Update");
                UE_LOG(LogTemp, Warning, TEXT("CharacterID %d의 PlayerStatusWidget에서 채팅 업데이트됨"), TargetCharacterID);
                return;
            }
        }
    }

}

void AMultiGameHUD::CreatePlayerWidgets()
{
    if (!WrapStatusWidgetClass) return;

    UWrapStatusWidget* NewItem = CreateWidget<UWrapStatusWidget>(GetWorld(), WrapStatusWidgetClass);
    if (NewItem)
    {
        NewItem->InitializePlayerStatus();
        NewItem->AddToViewport();
    }
 
}

void AMultiGameHUD::LogMessage(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
    }
}
