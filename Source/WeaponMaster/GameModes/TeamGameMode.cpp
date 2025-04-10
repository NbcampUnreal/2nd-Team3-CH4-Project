#include "GameModes/TeamGameMode.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"

void ATeamGameMode::SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController)
{
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
