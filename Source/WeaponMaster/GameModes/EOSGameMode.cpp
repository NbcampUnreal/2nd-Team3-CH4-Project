#include "EOSGameMode.h"

#include "Session/EOSGameSession.h"
#include "WeaponMaster/PlayerControllers/EOSPlayerController.h"


AEOSGameMode::AEOSGameMode()
{
	PlayerControllerClass = AEOSPlayerController::StaticClass();
	GameSessionClass = AEOSGameSession::StaticClass();
}

void AEOSGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (AEOSGameSession* MyGameSession = GetEOSGameSession())
	{
		MyGameSession->CreateSession();
	}
}

void AEOSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AEOSPlayerController* MyPC = Cast<AEOSPlayerController>(NewPlayer))
	{
		MyPC->BeginLoginProcess();
	}
}

