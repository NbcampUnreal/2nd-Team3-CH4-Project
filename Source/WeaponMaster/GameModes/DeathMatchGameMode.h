#pragma once

#include "CoreMinimal.h"
#include "BattleGMInterface.h"
#include "GameFramework/GameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "DeathMatchGameMode.generated.h"

class ABaseBattleCharacter;
/**
 * 개인전 데스매치 게임 모드
 */
UCLASS()
class WEAPONMASTER_API ADeathMatchGameMode : public AGameMode, public IBattleGMInterface
{
	GENERATED_BODY()

public:
	ADeathMatchGameMode();
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// BattleGMInterface 구현
	virtual void SpawnPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController) override;
	virtual void HandlePlayerDeath(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* Controller) override;
	
	UFUNCTION(BlueprintCallable, Category = "Instance")
	FORCEINLINE_DEBUGGABLE UWeaponMasterGameInstance* GetInstance() const
	{
		return Cast<UWeaponMasterGameInstance>(GetGameInstance());
	}
	
	// 게임 종료 시 결과 표시 및 우승자 결정
	UFUNCTION(BlueprintCallable, Category = "Game")
	void DisplayMatchResults();
	
	// 특정 플레이어의 킬 수 얻기
	UFUNCTION(BlueprintCallable, Category = "Game")
	int32 GetPlayerKills(AController* Controller) const;
	
private:
	bool HasCharacterSpawner() const;

	UPROPERTY()
	int32 TimerCountDown = 300;

	UFUNCTION()
	void SetTimer();

	UPROPERTY()
	FTimerHandle PlayCountDownTimerHandle;

	UFUNCTION()
	void PlayCountDownTimerAction();

	void UpdateAllPlayerHUDs();
    
	// 마지막 데미지를 준 컨트롤러 찾기 (Helper 함수)
	AController* FindLastDamagingController(ABaseBattleCharacter* Character);
	
	// 매치 설정
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	float RespawnDelay = 3.0f;
};