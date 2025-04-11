#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "WeaponMasterGameState.generated.h"

UCLASS()
class WEAPONMASTER_API AWeaponMasterGameState : public AGameState
{
	GENERATED_BODY()

public:
	AWeaponMasterGameState();
	
	FORCEINLINE_DEBUGGABLE void SetCooperateVotedPlayerNum(const bool IsVoted)
	{
		if (IsVoted)
		{
			++CooperateVotedPlayerNum;
			if (DeathMatchVotedPlayerNum > 0)
			{
				--DeathMatchVotedPlayerNum;
			}
		}
		else
		{
			++CooperateVotedPlayerNum;
		}
	}

	FORCEINLINE_DEBUGGABLE void SetDeathMatchVotedPlayerNum(const bool IsVoted)
	{
		if (IsVoted)
		{
			++DeathMatchVotedPlayerNum;
			if (CooperateVotedPlayerNum > 0)
			{
				--CooperateVotedPlayerNum;
			}
		}
		else
		{
			++DeathMatchVotedPlayerNum;
		}
	}

	FORCEINLINE_DEBUGGABLE int32 GetCooperateVotedPlayerNum() const { return CooperateVotedPlayerNum; }
	FORCEINLINE_DEBUGGABLE int32 GetDeathMatchVotedPlayerNum() const { return DeathMatchVotedPlayerNum; }
	FORCEINLINE_DEBUGGABLE bool IsCooperateVotedMore() const { return CooperateVotedPlayerNum > DeathMatchVotedPlayerNum ? true : false; }
	FORCEINLINE_DEBUGGABLE bool IsVotedEqual() const { return CooperateVotedPlayerNum == DeathMatchVotedPlayerNum ? true : false; }

private:
	UPROPERTY(Replicated)
	int32 CooperateVotedPlayerNum;

	UPROPERTY(Replicated)
	int32 DeathMatchVotedPlayerNum;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
