#include "GameModes/TeamGameMode.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "Kismet/GameplayStatics.h"
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

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawner::StaticClass(), FoundActors);

	uint8 cnt = 0;

	while (true)
	{
		
		uint8 RandomSpawnerIndex = FMath::RandRange(0, FoundActors.Num() - 1);
		ACharacterSpawner* CharacterSpawner = Cast<ACharacterSpawner>(FoundActors[RandomSpawnerIndex]);

		if (ACharacter* SpawnCharacter = CharacterSpawner->SpawnCharacter(CharacterClass))
		{
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

		// Place CharacterSpawners more than PlayerNumbers.
		// You need to consider character size not to overlap when character spawned.
		check(++cnt < 200);
	}
}

void ATeamGameMode::HandlePlayerDeath(AController* Controller)
{
}
