#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Session/EOSGameSession.h"
#include "EOSGameMode.generated.h"

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
};
