#include "WeaponMasterPlayerState.h"

#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "GameModes/TeamGameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Net/UnrealNetwork.h"

AWeaponMasterPlayerState::AWeaponMasterPlayerState()
{
	bReplicates = true;
	
	// 초기값 설정
	CharacterClass = nullptr;
	TeamIdx = 0;
	KillCount = 0;
	DeathCount = 0;
	TotalDamageDealt = 0.0f;
}



void AWeaponMasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	// 리플리케이션 등록
	DOREPLIFETIME(AWeaponMasterPlayerState, TeamIdx);
	DOREPLIFETIME(AWeaponMasterPlayerState, KillCount);
	DOREPLIFETIME(AWeaponMasterPlayerState, DeathCount);
	DOREPLIFETIME(AWeaponMasterPlayerState, TotalDamageDealt);
	DOREPLIFETIME(AWeaponMasterPlayerState, CharacterClass);
}

void AWeaponMasterPlayerState::AddKill()
{
	if (HasAuthority()) // 서버에서만 실행
	{
		KillCount++;
		UE_LOG(LogTemp, Display, TEXT("플레이어 %s의 킬 카운트 증가: %d"), *GetPlayerName(), KillCount);
	}
}

void AWeaponMasterPlayerState::AddDeath()
{
	if (HasAuthority()) // 서버에서만 실행
	{
		DeathCount++;
		UE_LOG(LogTemp, Display, TEXT("플레이어 %s의 사망 카운트 증가: %d"), *GetPlayerName(), DeathCount);
	}
}

void AWeaponMasterPlayerState::AddDamageDealt(float Damage)
{
	if (HasAuthority()) // 서버에서만 실행
	{
		TotalDamageDealt += Damage;
		UE_LOG(LogTemp, Display, TEXT("플레이어 %s의 총 데미지: %.2f"), *GetPlayerName(), TotalDamageDealt);
	}
}

void AWeaponMasterPlayerState::SetKillCount(int32 NewKillCount)
{
	if (HasAuthority()) // 서버에서만 실행
	{
		KillCount = NewKillCount;
		UE_LOG(LogTemp, Display, TEXT("플레이어 %s의 킬 카운트 설정: %d"), *GetPlayerName(), KillCount);
	}
}

void AWeaponMasterPlayerState::SetDeathCount(int32 NewDeathCount)
{
	if (HasAuthority()) // 서버에서만 실행
	{
		DeathCount = NewDeathCount;
		UE_LOG(LogTemp, Display, TEXT("플레이어 %s의 사망 카운트 설정: %d"), *GetPlayerName(), DeathCount);
	}
}

void AWeaponMasterPlayerState::SetTotalDamageDealt(float NewDamage)
{
	if (HasAuthority()) // 서버에서만 실행
	{
		TotalDamageDealt = NewDamage;
		UE_LOG(LogTemp, Display, TEXT("플레이어 %s의 총 데미지 설정: %.2f"), *GetPlayerName(), TotalDamageDealt);
	}
}

void AWeaponMasterPlayerState::OnRep_KillCount()
{
	OnKillCountChanged.Broadcast(this, KillCount);
}

void AWeaponMasterPlayerState::OnRep_DeathCount()
{
	OnDeathCountChanged.Broadcast(this, DeathCount);
}

void AWeaponMasterPlayerState::OnRep_TotalDamageCount()
{
	OnTotalDamageCountChanged.Broadcast(this, TotalDamageDealt);
}

void AWeaponMasterPlayerState::OnHealthChangeBroadcast(const float HP, const float MaxHP)
{
	OnHealthChanged.Broadcast(this, HP, MaxHP);
}
