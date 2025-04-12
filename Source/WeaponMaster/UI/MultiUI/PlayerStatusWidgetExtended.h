#pragma once

#include "CoreMinimal.h"
#include "UI/CommonUI/PlayerStatusWidget.h"
#include "PlayerStatusWidgetExtended.generated.h"

class UTextBlock;

/**
 * 개인전 모드에서 사용할 확장된 플레이어 상태 위젯
 * 기본 PlayerStatusWidget을 상속받아 킬, 데스, 점수 등의 추가 정보를 표시합니다.
 */
UCLASS()
class WEAPONMASTER_API UPlayerStatusWidgetExtended : public UPlayerStatusWidget
{
    GENERATED_BODY()
    
public:
    virtual void NativeConstruct() override;
    
    /** 킬 수 업데이트 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void UpdateKills(int32 NewKills);
    
    /** 데스 수 업데이트 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void UpdateDeaths(int32 NewDeaths);
    
    /** 점수 업데이트 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void UpdateScore(int32 NewScore);
    
    /** 킬 수 반환 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    int32 GetKills() const { return Kills; }
    
    /** 데스 수 반환 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    int32 GetDeaths() const { return Deaths; }
    
    /** 점수 반환 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    int32 GetScore() const { return Score; }
    
    /** 내 플레이어인지 설정 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void SetIsLocalPlayer(bool bIsLocal);
    
protected:
    // UI 구성요소 - BindWidget으로 블루프린트에서 연결
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> KillsText;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DeathsText;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ScoreText;
    
    // 데이터 속성
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    int32 Kills;
    
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    int32 Deaths;
    
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    int32 Score;
    
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    bool bIsLocalPlayer;
    
    /** 텍스트 업데이트 */
    void UpdateTexts();
};