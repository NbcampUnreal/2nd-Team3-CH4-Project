#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DeathMatchHUD.generated.h"

class UIndividualMatchStatusWidget;
class UTextBlock;
class UUserWidget;

/**
 * 개인전 모드를 위한 HUD 클래스
 */
UCLASS()
class WEAPONMASTER_API ADeathMatchHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ADeathMatchHUD();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void DrawHUD() override;
	
	/** HUD 초기화 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void InitializeHUD();
	
	/** 플레이어 상태 위젯 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void UpdatePlayerStats(int32 PlayerID, int32 Kills, int32 Deaths, int32 Score);
	
	/** 플레이어 체력 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void UpdatePlayerHealth(int32 PlayerID, float CurrentHealth, float MaxHealth);
	
	/** 남은 시간 설정 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void SetRemainingTime(int32 SecondsRemaining);
	
	/** 게임 종료 화면 표시 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void ShowGameOverScreen(bool bIsWinner, int32 TotalScore);
	
	/** HUD 숨기기/표시 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void SetHUDVisibility(bool bIsVisible);
	
	/** 인게임 메뉴 전환 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void ToggleInGameMenu();
	
	/** 메뉴 화면인지 확인 */
	UFUNCTION(BlueprintPure, Category = "DeathMatch HUD")
	bool IsMenuVisible() const;
	
protected:
	/** 개인전 상태 위젯 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UIndividualMatchStatusWidget> IndividualMatchStatusWidgetClass;
	
	/** 게임 종료 위젯 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	
	/** 인게임 메뉴 위젯 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> InGameMenuWidgetClass;
	
private:
	/** 개인전 상태 위젯 인스턴스 */
	UPROPERTY()
	UIndividualMatchStatusWidget* IndividualMatchStatusWidget;
	
	/** 게임 종료 위젯 인스턴스 */
	UPROPERTY()
	UUserWidget* GameOverWidget;
	
	/** 인게임 메뉴 위젯 인스턴스 */
	UPROPERTY()
	UUserWidget* InGameMenuWidget;
	
	/** 메뉴 표시 여부 */
	bool bIsMenuVisible;
	
	/** 플레이어 정렬 타이머 */
	FTimerHandle SortPlayersTimerHandle;
	
	/** 내 플레이어 ID */
	int32 LocalPlayerID;
};