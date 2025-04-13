// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../UIData/UISelectData.h"
#include "WeaponMasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API AWeaponMasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	// 싱글일때 0
	// 협력전일때 0
	// 팀전일때 0, 1
	// 리플리케이트 되도록
	UPROPERTY(Replicated)
	int32 TeamIdx;
	
	// 플레이어 전투 통계
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 KillCount;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 DeathCount;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	float TotalDamageDealt;

public:

	AWeaponMasterPlayerState();

	FUniqueNetIdRepl UniqueId;

	/** 팀 ID 반환 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	uint8 GetTeamID() const { return TeamIdx; }

	/** 팀 ID 설정 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetTeamID(uint8 NewTeamID) { TeamIdx = NewTeamID; }
	
	/** 킬 수 증가 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddKill();
	
	/** 킬 수 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetKillCount() const { return KillCount; }
	
	/** 죽은 수 증가 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddDeath();
	
	/** 죽은 수 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetDeathCount() const { return DeathCount; }
	
	/** 데미지 추가 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddDamageDealt(float Damage);
	
	/** 총 데미지 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetTotalDamageDealt() const { return TotalDamageDealt; }
	
	// 리플리케이션 오버라이드
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};