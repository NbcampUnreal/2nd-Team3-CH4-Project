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
	uint8 TeamIdx;
	
public:
	// FUniqueNetIdRepl UniqueId; 이거 PlayerState에 원래 있어요!
//
//	AWeaponMasterPlayerState();
//
//	UPROPERTY(ReplicatedUsing = OnRep_SelectionDataChanged)
//	FPlayerSelectionData SelectionData;
//	void SetCharacterSelection(int32 CharID);
//	void SetWeaponSelection(int32 WeaponID);
//	void SetMapSelection(int32 MapID);
//	void SetReadyStatus(bool IsReady);
//	void SetLastChatMessage(const FString& Message);
//
//	// Network replication setup
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//
//
//protected:
//	UFUNCTION()
//	void OnRep_SelectionDataChanged();
//
//	UFUNCTION()
//	void OnRep_ReadyStatusChanged();
};
