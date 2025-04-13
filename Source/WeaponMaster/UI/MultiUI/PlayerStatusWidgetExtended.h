#pragma once

#include "CoreMinimal.h"
#include "UI/CommonUI/PlayerStatusWidget.h"
#include "PlayerStatusWidgetExtended.generated.h"

class UTextBlock;

/**
 * Extended player status widget for use in different game modes
 * Extends the base PlayerStatusWidget with additional features like
 * kills, deaths tracking and local player highlighting.
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