#include "MultiGameHUD.h"
#include "WrapStatusWidget.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "../CommonUI/OptionMenuWidget.h"
#include "ChatWidget.h"
#include "PlayerNameWidget.h"
#include "SessionLobbyWidget.h"
#include "SessionWidget.h"

void AMultiGameHUD::BeginPlay()
{
	Super::BeginPlay();
    
    if (WrapStatusWidgetClass)
    {
        //CreatePlayerWidgets();
        // 맵 넘어가면 생성
    }
    
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMultiGameHUD::TestDummyModule, 5.0f, false);

    if (OptionMenuWidgetClass)
    {
        OptionMenuWidget = CreateWidget<UOptionMenuWidget>(GetOwningPlayerController(), OptionMenuWidgetClass);
        if (OptionMenuWidget)
        {
            OptionMenuWidget->SetVisibility(ESlateVisibility::Hidden);
            OptionMenuWidget->AddToViewport();
        }
    }
    
    if (ChatWidgetClass)
    {
        ChatWidget = CreateWidget<UChatWidget>(GetOwningPlayerController(), ChatWidgetClass);
        if (OptionMenuWidget)
        {
            ChatWidget->SetVisibility(ESlateVisibility::Hidden);
            ChatWidget->AddToViewport();
        }
    }

    if (SessionLobbyWidgetClass)
    {
        SessionLobbyWidget = CreateWidget<USessionLobbyWidget>(GetWorld(), SessionLobbyWidgetClass);
        if (SessionLobbyWidget)
        {
            SessionLobbyWidget->AddToViewport();
            SessionLobbyWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (MapSelectWidgetClass)
    {
        MapSelectWidget = CreateWidget<USessionWidget>(GetWorld(), MapSelectWidgetClass);
        if (MapSelectWidget)
        {
            MapSelectWidget->AddToViewport();
        }
    } 
    
    // 임시
    if (PlayerNameWidgetClass)
    {
        PlayerNameWidget = CreateWidget<UPlayerNameWidget>(GetWorld(), PlayerNameWidgetClass);
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

