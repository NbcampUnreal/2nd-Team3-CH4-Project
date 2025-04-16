#include "GameModes/TeamGameMode.h"

#include "Characters/Components/IBattleSystemUser.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/EOSPlayerController.h"
#include "PlayerControllers/WeaponMasterController.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"

void ATeamGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetTimer();
}

void ATeamGameMode::SpawnPlayerCharacter(APlayerController* Controller)
{
	Super::SpawnPlayerCharacter(Controller);

	UE_LOG(LogTemp, Warning, TEXT("ATeamGameMode::SpawnPlayerCharacter : Called."));
	
	if (!HasCharacterSpawner()) return;
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawner::StaticClass(), FoundActors);

	uint32 Cnt = 50000;

	auto CharacterClassRandomIndex = FMath::RandRange(0, CharacterClasses.Num() - 1);
	auto ItemNameRandomIndex = FMath::RandRange(0, ItemNames.Num() - 1);
	
	while (--Cnt)
	{
		uint8 RandomSpawnerIndex = FMath::RandRange(0, FoundActors.Num() - 1);
		ACharacterSpawner* CharacterSpawner = Cast<ACharacterSpawner>(FoundActors[RandomSpawnerIndex]);

		if (ACharacter* SpawnCharacter = CharacterSpawner->SpawnCharacter(CharacterClasses[CharacterClassRandomIndex]))
		{
			if (AWeaponMasterController* WMPC = Cast<AWeaponMasterController>(Controller))
			{
				// Possess할때 Owner설정도 됨!
				// SpawnCharacter->SetOwner(WMPC);
				WMPC->Possess(SpawnCharacter);
				UE_LOG(LogTemp, Warning, TEXT("Possessed Pawn: %s"), *WMPC->GetPawn()->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : Possess Failed."));
			}
			
			if (SpawnCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
			{
				UItemComponent* ItemComponent = IBattleSystemUser::Execute_GetItemComponent(SpawnCharacter);
				ItemComponent->EquipItem(ItemNames[ItemNameRandomIndex]);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : SpawnCharacter doesn't implement IBattleSystemUser."));
			}
		
			break;
		}
		
	}
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
