#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DeathMatchHUD.generated.h"

class AWeaponMasterPlayerState;
class UKillLogWidget;
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

	/** 개인전 상태 위젯 인스턴스 */
	UPROPERTY()
	UIndividualMatchStatusWidget* IndividualMatchStatusWidget;
	
	/** HUD 초기화 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void InitializeHUD();
	
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

	/** 킬로그 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void UpdateKillLog(const FText& Killer, const FText& Victim) const;

	/** 플레이어 상태 위젯 생성 */
	UFUNCTION(BlueprintCallable, Category = "DeathMatch HUD")
	void BindPlayerStatusWidget(AWeaponMasterPlayerState* WMPS);

	/** 킬카운트 증가 */
	UFUNCTION()
	void UpdateKillCount(AWeaponMasterPlayerState* OwnerPS, int32 NewVal);

	/** 데스카운트 증가 */
	UFUNCTION()
	void UpdateDeathCount(AWeaponMasterPlayerState* OwnerPS, int32 NewVal);
	
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
	
	/** 게임 종료 위젯 인스턴스 */
	UPROPERTY()
	UUserWidget* GameOverWidget;
	
	/** 인게임 메뉴 위젯 인스턴스 */
	UPROPERTY()
	UUserWidget* InGameMenuWidget;

private:
	/** 메뉴 표시 여부 */
	UPROPERTY()
	bool bIsMenuVisible;
	
	/** 내 플레이어 ID */
	UPROPERTY()
	int32 LocalPlayerID;

	/** 내 플레이어 이름 */
	UPROPERTY()
	FString LocalPlayerName;

	UPROPERTY()
	TSet<AWeaponMasterPlayerState*> BoundPlayerStates;

	/** PS 바인딩 */
	UFUNCTION()
	void TryBindPlayerState();
};