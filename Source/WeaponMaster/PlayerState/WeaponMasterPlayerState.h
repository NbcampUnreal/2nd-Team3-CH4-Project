#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "WeaponMasterPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, AWeaponMasterPlayerState*, OwnerPS, int32, NewValue);

UCLASS()
class WEAPONMASTER_API AWeaponMasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AWeaponMasterPlayerState();

	UPROPERTY(Replicated)
	TSubclassOf<ACharacter> CharacterClass;
	
	UPROPERTY(Replicated)
	int32 TeamIdx;
	
	UPROPERTY(ReplicatedUsing=OnRep_KillCount, BlueprintReadOnly, Category = "Stats")
	int32 KillCount;

	UFUNCTION()
	void OnRep_KillCount();

	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnKillCountChanged;
	
	UPROPERTY(ReplicatedUsing=OnRep_DeathCount, BlueprintReadOnly, Category = "Stats")
	int32 DeathCount;

	UFUNCTION()
	void OnRep_DeathCount();

	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnDeathCountChanged;
	
	UPROPERTY(ReplicatedUsing=OnRep_TotalDamageCount, BlueprintReadOnly, Category = "Stats")
	float TotalDamageDealt;

	UFUNCTION()
	void OnRep_TotalDamageCount();

	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnTotalDamageCountChanged;

	/** 팀 ID 반환 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	uint8 GetTeamID() const { return TeamIdx; }

	/** 팀 ID 설정 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	FORCEINLINE_DEBUGGABLE void SetTeamID(const uint8 NewTeamID) { TeamIdx = NewTeamID; }
	
	/** 킬 수 증가 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddKill();
	
	/** 킬 수 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE_DEBUGGABLE int32 GetKillCount() const { return KillCount; }
	
	/** 죽은 수 증가 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddDeath();
	
	/** 죽은 수 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE_DEBUGGABLE int32 GetDeathCount() const { return DeathCount; }
	
	/** 데미지 추가 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddDamageDealt(float Damage);
	
	/** 총 데미지 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE_DEBUGGABLE float GetTotalDamageDealt() const { return TotalDamageDealt; }

	// 킬 카운트 설정
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetKillCount(int32 NewKillCount);

	// 데스 카운트 설정
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetDeathCount(int32 NewDeathCount);

	// 총 데미지 설정
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetTotalDamageDealt(float NewDamage);
	
	// 리플리케이션 오버라이드
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};