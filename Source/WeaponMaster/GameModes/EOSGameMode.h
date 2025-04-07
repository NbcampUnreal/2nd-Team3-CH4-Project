#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Session/EOSGameSession.h"
#include "EOSGameMode.generated.h"

USTRUCT(BlueprintType)
struct FSinglePlayerController
{
	GENERATED_BODY()

	TSharedPtr<APlayerController> Controller;
};

USTRUCT(BlueprintType)
struct FPlayerControllerCollection
{
	GENERATED_BODY()

	TArray<FSinglePlayerController> Controller;
};

UCLASS()
class WEAPONMASTER_API AEOSGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AEOSGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category = "Session")
	FORCEINLINE_DEBUGGABLE AEOSGameSession* GetEOSGameSession() const { return Cast<AEOSGameSession>(GameSession); }
private:
	UPROPERTY()
	FPlayerControllerCollection PlayerControllerList;
};
