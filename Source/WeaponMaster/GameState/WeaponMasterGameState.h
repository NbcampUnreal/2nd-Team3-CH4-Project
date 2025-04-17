#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "WeaponMasterGameState.generated.h"

UCLASS()
class WEAPONMASTER_API AWeaponMasterGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AWeaponMasterGameState();


	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChatMessage(const FText& Message);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsPVP;
	
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
	UPROPERTY(ReplicatedUsing = OnRep_PlayerNumChanged)
	int32 CooperateVotedPlayerNum;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerNumChanged)
	int32 DeathMatchVotedPlayerNum;

	UFUNCTION()
	void OnRep_PlayerNumChanged() const;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
