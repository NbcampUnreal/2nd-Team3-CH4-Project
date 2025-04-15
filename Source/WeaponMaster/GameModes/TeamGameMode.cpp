#include "GameModes/TeamGameMode.h"
#include "PlayerControllers/EOSPlayerController.h"

void ATeamGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetTimer();
}

void ATeamGameMode::SetTimer()
{
	GetWorldTimerManager().SetTimer(
		PlayCountDownTimerHandle,
		this,
		&ATeamGameMode::PlayCountDownTimerAction,
		1.0f,
		true
	);
}

void ATeamGameMode::PlayCountDownTimerAction()
{
	if (TimerCountDown > 0 || TimerCountDown != 0)
	{
		--TimerCountDown;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AEOSPlayerController* PlayerController = Cast<AEOSPlayerController>(Iterator->Get()))
		{
			PlayerController->Client_UpdateInGameTimer(TimerCountDown);
		}
	}

	if (TimerCountDown == 0)
	{
		// Score Board 이동
		
		GetWorldTimerManager().ClearTimer(PlayCountDownTimerHandle);
	}
}
