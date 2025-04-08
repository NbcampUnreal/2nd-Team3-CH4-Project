#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WeaponMasterGameInstance.generated.h"

UCLASS()
class WEAPONMASTER_API UWeaponMasterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 플레이어 관리 필요
	UWeaponMasterGameInstance();

	void Login();

	void HandleLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	FDelegateHandle LoginDelegateHandle;

	void FindSessions(FName SearchKey = "DEDICATEDONLY", FString SearchValue = "KeyValue");
	void HandleFindSessionsCompleted(bool bWasSuccessful, TSharedRef<FOnlineSessionSearch> Search);

	FDelegateHandle FindSessionsDelegateHandle;
	FString ConnectString;

	FOnlineSessionSearchResult* SessionToJoin;
	void JoinSession();
 
	void HandleJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	FDelegateHandle JoinSessionDelegateHandle;
};
