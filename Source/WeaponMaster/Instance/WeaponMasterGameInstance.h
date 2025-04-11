#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WeaponMasterGameInstance.generated.h"

UENUM(BlueprintType)
enum class EPlayerEOSStateType : uint8
{
	Login			UMETA(DisplayName = "Login"),
	FindSession     UMETA(DisplayName = "FindSession"),
	JoinSession     UMETA(DisplayName = "JoinSession")
};

UENUM(BlueprintType)
enum class ESessionResultType : uint8
{
	Success     UMETA(DisplayName = "Success"),
	Fail        UMETA(DisplayName = "Fail"),
	Unknown     UMETA(DisplayName = "Unknown")
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnProcessReturnValue, EPlayerEOSStateType, ESessionResultType);

UCLASS()
class WEAPONMASTER_API UWeaponMasterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 플레이어 관리 필요
	UWeaponMasterGameInstance();

	FOnProcessReturnValue OnProcessReturnValue;

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

	UFUNCTION(BlueprintCallable, Category = "EOS")
	FString GetPlayerName() const;

	UFUNCTION(BlueprintCallable, Category = "EOS")
	bool IsPlayerLoggedIn() const;
  
	// 언리얼 서버 디버그 에디터에 프로세스 연결
	
	
	// 선택한 캐릭터, 아이템 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<ACharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	FName ItemName;
};
