#include "MultiGameHUD.h"
#include "WrapStatusWidget.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "../CommonUI/OptionMenuWidget.h"
#include "ChatWidget.h"
#include "SessionWidget.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "GameFramework/PlayerState.h"
#include "GameState/WeaponMasterGameState.h"
#include "PlayerControllers/EOSPlayerController.h"
#include "PlayerState/WeaponMasterPlayerState.h"

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
                if (WrapStatusWidget)
                {
                    WrapStatusWidget->SetVisibility(ESlateVisibility::Hidden);
                }
                
                if (MapSelectWidget)
                {
                    MapSelectWidget->SetVisibility(ESlateVisibility::Visible);
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

void AMultiGameHUD::UpdatePlayerStatus(int32 TargetCharacterID, const FPlayerStatusInfo& StatusInfo)
{
    if (!WrapStatusWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("WrapStatusWidget 인스턴스가 존재하지 않습니다."));
        return;
    }

    // 플레이어 상태 위젯 찾기
    UPlayerStatusWidget* TargetWidget = GetPlayerStatusWidget(TargetCharacterID);
    
    if (TargetWidget)
    {
        // 한 번에 모든 정보 업데이트
        TargetWidget->UpdatePlayerStatus(StatusInfo);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("CharacterID %d의 PlayerStatusWidget을 찾을 수 없습니다."), TargetCharacterID);
}

void AMultiGameHUD::RefreshAllPlayerStatus()
{
    // 게임 상태와 플레이어 상태 정보를 사용하여 모든 위젯 업데이트
    AWeaponMasterGameState* GameState = GetWorld()->GetGameState<AWeaponMasterGameState>();
    if (!GameState)
    {
        return;
    }
    
    TArray<APlayerState*> PlayerStates = GameState->PlayerArray;
    for (APlayerState* PS : PlayerStates)
    {
        AWeaponMasterPlayerState* WMPS = Cast<AWeaponMasterPlayerState>(PS);
        if (!WMPS)
        {
            continue;
        }
        
        // 플레이어 상태 정보 구성
        FPlayerStatusInfo StatusInfo;
        StatusInfo.PlayerName = PS->GetPlayerName();
        StatusInfo.TeamID = WMPS->GetTeamID();
        StatusInfo.CharacterID = PS->GetPlayerId();
        
        // 플레이어 캐릭터 정보 가져오기
        ABaseBattleCharacter* Character = Cast<ABaseBattleCharacter>(PS->GetPawn());
        if (Character)
        {
            StatusInfo.CurrentHealth = Character->GetHP();
            StatusInfo.MaxHealth = Character->GetMaxHP();
            StatusInfo.PlayerThumbnailTexture = Character->GetCharacterThumbnail();
        }
        
        // 위젯 업데이트
        UpdatePlayerStatus(PS->GetPlayerId(), StatusInfo);
    }
}

UPlayerStatusWidget* AMultiGameHUD::GetPlayerStatusWidget(int32 CharacterID)
{
    if (!WrapStatusWidget)
    {
        return nullptr;
    }
    
    // 왼쪽 팀 컨테이너에서 검색
    TArray<UWidget*> LeftWidgets = WrapStatusWidget->GetLeftTeamContainer()->GetAllChildren();
    for (UWidget* Widget : LeftWidgets)
    {
        if (UPlayerStatusWidget* PSWidget = Cast<UPlayerStatusWidget>(Widget))
        {
            if (PSWidget->GetCharacterID() == CharacterID)
            {
                return PSWidget;
            }
        }
    }
    
    // 오른쪽 팀 컨테이너에서 검색
    TArray<UWidget*> RightWidgets = WrapStatusWidget->GetRightTeamContainer()->GetAllChildren();
    for (UWidget* Widget : RightWidgets)
    {
        if (UPlayerStatusWidget* PSWidget = Cast<UPlayerStatusWidget>(Widget))
        {
            if (PSWidget->GetCharacterID() == CharacterID)
            {
                return PSWidget;
            }
        }
    }
    
    return nullptr;
}

