#include "PlayerStatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

void UPlayerStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 기본값 초기화
    CharacterID = -1;
    TeamID = 0;
}

void UPlayerStatusWidget::UpdatePlayerStatus(const FPlayerStatusInfo& StatusInfo)
{
    // 모든 플레이어 정보를 한번에 업데이트
    SetPlayerName(StatusInfo.PlayerName);
    UpdateHealth(StatusInfo.CurrentHealth, StatusInfo.MaxHealth);
    SetThumbnail(StatusInfo.PlayerThumbnailTexture);
    SetID(StatusInfo.CharacterID);
    SetTeamID(StatusInfo.TeamID);
}

void UPlayerStatusWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar)
    {
        // 0과 1 사이의 값으로 정규화하여 ProgressBar에 설정
        float HealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
        HealthBar->SetPercent(HealthPercent);
        
        // 체력이 낮을 때 색상 변경 (옵션)
        if (HealthPercent < 0.3f)
        {
            // 빨간색으로 변경 (위험 상태)
            HealthBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.2f, 0.2f));
        }
        else if (HealthPercent < 0.6f)
        {
            // 노란색으로 변경 (주의 상태)
            HealthBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.8f, 0.0f));
        }
        else
        {
            // 녹색으로 변경 (정상 상태)
            HealthBar->SetFillColorAndOpacity(FLinearColor(0.0f, 0.8f, 0.2f));
        }
    }
}

void UPlayerStatusWidget::SetPlayerName(const FString& Name)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(Name));
    }
}

void UPlayerStatusWidget::SetThumbnail(UTexture2D* Thumbnail)
{
    if (PlayerThumbnail && Thumbnail)
    {
        PlayerThumbnail->SetBrushFromTexture(Thumbnail);
    }
}

void UPlayerStatusWidget::SetID(int32 NewID)
{
    CharacterID = NewID;
}

void UPlayerStatusWidget::SetTeamID(int32 NewTeamID)
{
    TeamID = NewTeamID;
}