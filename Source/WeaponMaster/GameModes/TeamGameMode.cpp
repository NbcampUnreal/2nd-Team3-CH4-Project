#include "GameModes/TeamGameMode.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "PlayerControllers/EOSPlayerController.h"

void ATeamGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetTimer();
}

bool ATeamGameMode::HasCharacterSpawner() const
{
	for (TActorIterator<ACharacterSpawner> It(GetWorld()); It; ++It)
	{
		return true; 
	}
	return false;
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

void ATeamGameMode::SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController)
{
	if (!HasCharacterSpawner()) return;
	bool bSuccessFlag = false;
	while (true)
	{
		for (TActorIterator<ACharacterSpawner> It(GetWorld()); It; ++It)
		{
			ACharacterSpawner* CharacterSpawner = *It;
			
			if (ACharacter* SpawnCharacter = CharacterSpawner->SpawnCharacter(CharacterClass))
			{
				bSuccessFlag = true;

				SpawnCharacter->SetOwner(OwnerController);
				OwnerController->Possess(SpawnCharacter);

				if (SpawnCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
				{
					UItemComponent* ItemComponent = IBattleSystemUser::Execute_GetItemComponent(SpawnCharacter);
					ItemComponent->EquipItem(ItemName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : SpawnCharacter doesn't implement IBattleSystemUser."));
				}
				
				break;
			}
		}
		if (bSuccessFlag) break;
	}
}
