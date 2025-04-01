#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "gls_ExampleGameMode.generated.h"

/**
 * This class is created to demonstrate the possibilities of using GLS macroses. 
 * To open the GLS overlay in the editor or game, hold down the key combination ALT + SHIFT + G
 */
UCLASS()
class GLS_API Agls_ExampleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	Agls_ExampleGameMode();

	virtual void BeginPlay() override;

	void PrintWithoutContextConst() const;

	void PrintToGLS();

	void Tick(float DeltaSeconds) override;
};
