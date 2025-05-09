#pragma once

#include "CoreMinimal.h"
#include "UI/CommonUI/PlayerStatusWidget.h"
#include "PlayerStatusWidgetExtended.generated.h"

class UTextBlock;

/**
 * 다양한 게임 모드에서 사용할 수 있는 확장된 플레이어 상태 위젯
 * 기본 PlayerStatusWidget을 다음과 같은 추가 기능으로 확장합니다.
 * 킬, 사망 추적 및 로컬 플레이어 강조 표시.
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
    
    /** 킬 수 반환 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    int32 GetKills() const { return Kills; }
    
    /** 데스 수 반환 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    int32 GetDeaths() const { return Deaths; }
    
    /** Set whether this widget represents the local player */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void SetIsLocalPlayer(bool bIsLocal);
    
protected:
    // UI components - to be bound with BindWidget in blueprint
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> KillsText;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DeathsText;
    
    // 데이터 속성
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    int32 Kills;
    
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    int32 Deaths;
    
    /** Whether this widget represents the local player */
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    bool bIsLocalPlayer;
    
    /** 텍스트 업데이트 */
    void UpdateTexts();
};