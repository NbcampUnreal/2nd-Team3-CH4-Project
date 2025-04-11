#include "MultiGameHUD.h"
#include "WrapStatusWidget.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "../CommonUI/OptionMenuWidget.h"
#include "ChatWidget.h"
#include "SessionWidget.h"
#include "PlayerControllers/EOSPlayerController.h"

void AMultiGameHUD::BeginPlay()
{
	Super::BeginPlay();
    
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMultiGameHUD::TestDummyModule, 5.0f, false);

    // 환경설정
    if (OptionMenuWidgetClass)
    {
        OptionMenuWidget = CreateWidget<UOptionMenuWidget>(GetOwningPlayerController(), OptionMenuWidgetClass);
        if (OptionMenuWidget)
        {
            OptionMenuWidget->SetVisibility(ESlateVisibility::Hidden);
            OptionMenuWidget->AddToViewport();
        }
    }

    // 채팅 (PVP, PVE)
    if (ChatWidgetClass)
    {
        ChatWidget = CreateWidget<UChatWidget>(GetOwningPlayerController(), ChatWidgetClass);
        if (ChatWidget)
        {
            ChatWidget->SetVisibility(ESlateVisibility::Hidden);
            ChatWidget->AddToViewport();
        }
    }

    // 맵선택
    if (MapSelectWidgetClass)
    {
        MapSelectWidget = CreateWidget<USessionWidget>(GetWorld(), MapSelectWidgetClass);
        if (MapSelectWidget)
        {
            ChatWidget->SetVisibility(ESlateVisibility::Hidden);
            MapSelectWidget->AddToViewport();
        }
    }

    // 플레이어 상태 위젯
    if (WrapStatusWidgetClass)
    {
        WrapStatusWidget = CreateWidget<UWrapStatusWidget>(GetOwningPlayerController(), WrapStatusWidgetClass);
        if (WrapStatusWidget)
        {
            WrapStatusWidget->SetVisibility(ESlateVisibility::Hidden);
            WrapStatusWidget->InitializePlayerStatus();
            WrapStatusWidget->AddToViewport();
        }
    }

    TransferHUDBy(EMapType::SessionMap);
}

void AMultiGameHUD::TransferHUDBy(const EMapType MapType)
{
    switch (MapType)
    {
        case EMapType::PVPMap:
            {
                if (MapSelectWidget)
                {
                    MapSelectWidget->SetVisibility(ESlateVisibility::Hidden);
                }

                if (WrapStatusWidget)
                {
                    WrapStatusWidget->SetVisibility(ESlateVisibility::Visible);
                }
                break;
            }
        case EMapType::PVEMap:
            {
                if (MapSelectWidget)
                {
                    MapSelectWidget->SetVisibility(ESlateVisibility::Hidden);
                }

                if (WrapStatusWidget)
                {
                    WrapStatusWidget->SetVisibility(ESlateVisibility::Visible);
                }
                break;
            }
        case EMapType::SessionMap:
            {
                if (MapSelectWidget)
                {
                    ChatWidget->SetVisibility(ESlateVisibility::Visible);
                }
                
                if (WrapStatusWidget)
                {
                    WrapStatusWidget->SetVisibility(ESlateVisibility::Hidden);
                }
                break;
            }
    }
}

void AMultiGameHUD::TestDummyModule()
{
    if (!WrapStatusWidgetClass) return;
    
    WrapStatusWidget = CreateWidget<UWrapStatusWidget>(GetWorld(), WrapStatusWidgetClass);
    if (WrapStatusWidget)
    {
        WrapStatusWidget->InitializeDummyPlayerStatus(TestInt,1);
        WrapStatusWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
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

void AMultiGameHUD::LogMessage(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
    }
}

void AMultiGameHUD::SetHPModule(float NewHP,int32 TargetCharacterID)
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
                PSWidget->UpdateHealth(NewHP,100.f);
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
                PSWidget->UpdateHealth(NewHP, 100.f);
                LogMessage("Right Update");
                UE_LOG(LogTemp, Warning, TEXT("CharacterID %d의 PlayerStatusWidget에서 채팅 업데이트됨"), TargetCharacterID);
                return;
            }
        }
    }

}

void AMultiGameHUD::SetMenuWidget(bool bIsOpen)
{
    OptionMenuWidget->SetVisibility(bIsOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

