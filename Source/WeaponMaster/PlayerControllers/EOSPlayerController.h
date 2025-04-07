#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EOSPlayerController.generated.h"

class FOnlineSessionSearch;
class FOnlineSessionSearchResult;

UCLASS()
class WEAPONMASTER_API AEOSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEOSPlayerController();
	
	virtual void BeginPlay() override;

	void BeginLoginProcess();
};
