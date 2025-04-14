#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IndividualMatchStatusWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UPlayerStatusWidget;
class UScrollBox;

/**
 * 개인전 경기에서 모든 플레이어의 상태를 표시하는 위젯
 * 각 플레이어의 스코어, 킬/데스 정보를 표시합니다.
 */
UCLASS()
class WEAPONMASTER_API UIndividualMatchStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** 플레이어 상태 위젯 초기화 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitializePlayerStatus();

	/** 테스트용 더미 플레이어 상태 초기화 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitializeDummyPlayerStatus(int32 TotalPlayers, int32 LocalPlayerID = -1);

	/** 특정 플레이어의 킬 점수 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerKills(int32 PlayerID, int32 Kills);
	
	/** 특정 플레이어의 데스 점수 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerDeaths(int32 PlayerID, int32 Deaths);

	/** 특정 플레이어의 체력 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerHealth(int32 PlayerID, float CurrentHealth, float MaxHealth);

	/** 특정 플레이어의 상태 표시 여부 설정 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetPlayerVisibility(int32 PlayerID, bool bIsVisible);

	/** 플레이어 리스트 정렬 (점수 기준) */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SortPlayersByScore();

	/** 플레이어 컨테이너 반환 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	UScrollBox* GetPlayerContainer() const { return PlayerContainer; }

	/** 남은 시간 설정 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetRemainTimer(int32 TimeRemain) const;

protected:
	virtual void NativeConstruct() override;

	/** 모든 플레이어 컨테이너 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> PlayerContainer;

	/** 남은 시간 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainTimerTextBlock;

	/** 매치 이름 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MatchTitleTextBlock;

	/** 플레이어 상태 위젯 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerStatusWidget> PlayerStatusWidgetClass;

	/** 위젯 간 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float WidgetSpacing = 5.0f;

	/** 내 플레이어 ID */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	int32 MyPlayerID = -1;

	/** 플레이어 상태 위젯 맵 (플레이어 ID를 키로 사용) */
	UPROPERTY(Transient)
	TMap<int32, UPlayerStatusWidget*> PlayerWidgets;

private:
	/** 플레이어 위젯 제거 */
	void ClearPlayerWidgets();

	/** 최대 표시 플레이어 수 */
	static const int32 MAX_VISIBLE_PLAYERS = 16;
};