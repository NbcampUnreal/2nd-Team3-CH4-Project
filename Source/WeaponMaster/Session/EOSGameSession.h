#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "EOSGameSession.generated.h"

namespace EEndPlayReason
{
	enum Type : int;
}

DECLARE_MULTICAST_DELEGATE_TwoParams(OnProcessReturnValue, EMyStateType, EMyResultType);

UCLASS(Blueprintable)
class WEAPONMASTER_API AEOSGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	AEOSGameSession();

	OnProcessReturnValue OnProcessReturnValue;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool ProcessAutoLogin() override;

	virtual FString ApproveLogin(const FString& Options) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void NotifyLogout(const APlayerController* ExitingPlayer) override;

	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite=false) override;
	virtual void UnregisterPlayer(const APlayerController* ExitingPlayer) override;
	
	FName SessionName = "SessionName"; 
	const int MaxNumberOfPlayersInSession = 50;
	int NumberOfPlayersInSession;

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void CreateSession(FName KeyName = "DEDICATEDONLY", FString KeyValue= "KeyValue");
	void HandleCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	
	FDelegateHandle CreateSessionDelegateHandle;
	bool bSessionExists = false;
	
	void HandleRegisterPlayerCompleted(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccesful);
	FDelegateHandle RegisterPlayerDelegateHandle;
	
	void HandleUnregisterPlayerCompleted(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccesful);
	FDelegateHandle UnregisterPlayerDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void StartSession(); 
	void HandleStartSessionCompleted(FName SessionName, bool bWasSuccessful); 
	FDelegateHandle StartSessionDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void EndSession();
	void HandleEndSessionCompleted(FName SessionName, bool bWasSuccessful);
	FDelegateHandle EndSessionDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void DestroySession();
	void HandleDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	FDelegateHandle DestroySessionDelegateHandle; 
};
