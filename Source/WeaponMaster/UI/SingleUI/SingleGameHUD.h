#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "../CommonUI/PlayerStatusWidget.h" // FPlayerStatusInfo 구조체 사용을 위한 추가
#include "SingleGameHUD.generated.h"

class UPlayerStatusWidget;
class UOptionWidget;

UCLASS()
class WEAPONMASTER_API ASingleGameHUD : public AHUD
{
    GENERATED_BODY()
public:
    ASingleGameHUD();

    virtual void BeginPlay() override;
    
    /** 싱글게임 UI 위젯 초기화 */
    void OpenSingleWidget();
    
    /** 플레이어 HP 업데이트 (기존 메서드) */
    void SetPlayerHP(float NewHP);
    
    /** 옵션 메뉴 표시 여부 설정 */
    void SetMenuWidget(bool bIsOpen);
    
    /** 플레이어 상태 정보 전체 업데이트 */
    UFUNCTION(BlueprintCallable, Category = "UI|PlayerStatus")
    void UpdatePlayerStatus(const FPlayerStatusInfo& StatusInfo);
    
    /** 플레이어 상태 위젯 가져오기 */
    UFUNCTION(BlueprintCallable, Category = "UI|PlayerStatus")
    UPlayerStatusWidget* GetPlayerStatusWidget() const { return PlayerStatusWidget; }
    
    /** 플레이어 썸네일 설정 */
    UFUNCTION(BlueprintCallable, Category = "UI|PlayerStatus")
    void SetPlayerThumbnail(UTexture2D* Thumbnail);
    
    /** 플레이어 이름 설정 */
    UFUNCTION(BlueprintCallable, Category = "UI|PlayerStatus")
    void SetPlayerName(const FString& PlayerName);
    

    
    /** 채팅 메시지 표시 */
    UFUNCTION(BlueprintCallable, Category = "UI|PlayerStatus")
    void ShowChatMessage(const FString& Message);

protected:
    /** 플레이어 상태 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UPlayerStatusWidget> PlayerStatusWidgetClass;

    /** 플레이어 상태 위젯 인스턴스 */
    UPROPERTY()
    UPlayerStatusWidget* PlayerStatusWidget;

    /** 옵션 메뉴 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UOptionWidget> OptionMenuWidgetClass;

public:
    /** 옵션 메뉴 위젯 인스턴스 */
    UPROPERTY()
    UOptionWidget* OptionMenuWidget;

    /** 옵션 메뉴 표시 여부 */
    bool bIsOptionMenuVisible = false;
};