#include "SingleGameHUD.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "../CommonUI/OptionMenuWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "../../Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "../../Data/StatusTypes.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

ASingleGameHUD::ASingleGameHUD()
{
    // 생성자 초기화
}

void ASingleGameHUD::BeginPlay()
{
    Super::BeginPlay();
    
    // 옵션 메뉴 위젯 생성
    if (OptionMenuWidgetClass)
    {
        OptionMenuWidget = CreateWidget<UOptionMenuWidget>(GetOwningPlayerController(), OptionMenuWidgetClass);
        if (OptionMenuWidget)
        {
            OptionMenuWidget->SetVisibility(ESlateVisibility::Hidden);
            OptionMenuWidget->AddToViewport();
        }
    }
    
    // 입력 모드 설정
    if (APlayerController* PC = GetOwningPlayerController())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
    
    // 싱글 플레이어 위젯 초기화
    OpenSingleWidget();
}

void ASingleGameHUD::OpenSingleWidget()
{
    if (PlayerStatusWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(GetOwningPlayerController(), PlayerStatusWidgetClass);
        if (PlayerStatusWidget)
        {
            // 기본 정보로 초기화
            PlayerStatusWidget->SetPlayerName("Player");
            PlayerStatusWidget->UpdateHealth(100.f, 100.f);
            PlayerStatusWidget->SetID(0); // 싱글 플레이어는 ID 0으로 설정
            
            // 플레이어 캐릭터 정보 가져오기 시도
            if (APlayerController* PC = GetOwningPlayerController())
            {
                if (ABaseBattleCharacter* Character = Cast<ABaseBattleCharacter>(PC->GetPawn()))
                {
                    // 플레이어 상태 정보 구성
                    FPlayerStatusInfo StatusInfo;
                    StatusInfo.PlayerName = PC->PlayerState ? PC->PlayerState->GetPlayerName() : FString("Player");
                    StatusInfo.CurrentHealth = Character->GetHP();
                    StatusInfo.MaxHealth = Character->GetMaxHP();
                    StatusInfo.PlayerThumbnailTexture = Character->GetCharacterThumbnail();
                    StatusInfo.CharacterID = 0;
                    StatusInfo.TeamID = 0;
                
                    
                    // 플레이어 상태 위젯 업데이트
                    PlayerStatusWidget->UpdatePlayerStatus(StatusInfo);
                }
            }
            
            // 뷰포트에 추가
            PlayerStatusWidget->AddToViewport();
        }
    }
}

void ASingleGameHUD::SetPlayerHP(float NewHP)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->UpdateHealth(NewHP, 100.f);
    }
}

void ASingleGameHUD::SetMenuWidget(bool bIsOpen)
{
    if (OptionMenuWidget)
    {
        OptionMenuWidget->SetVisibility(bIsOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        bIsOptionMenuVisible = bIsOpen;
    }
}

void ASingleGameHUD::UpdatePlayerStatus(const FPlayerStatusInfo& StatusInfo)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->UpdatePlayerStatus(StatusInfo);
    }
}

void ASingleGameHUD::SetPlayerThumbnail(UTexture2D* Thumbnail)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetThumbnail(Thumbnail);
    }
}

void ASingleGameHUD::SetPlayerName(const FString& PlayerName)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetPlayerName(PlayerName);
    }
}



void ASingleGameHUD::ShowChatMessage(const FString& Message)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->UpdateChat(Message);
    }
}