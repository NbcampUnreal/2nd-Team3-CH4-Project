#pragma once

#include "CoreMinimal.h"
#include "Menus.generated.h"

UENUM(BlueprintType)
enum class EMenus : uint8
{
	Main UMETA(DisplayName = "Main"),
	CharacterSelect UMETA(DisplayName = "Character Select"),
	MapSelect UMETA(DisplayName = "Map Select"),
	Online UMETA(DisplayName = "Online"),
	Host UMETA(DisplayName = "Host"),
	Join UMETA(DisplayName = "Join"),
	Lobby UMETA(DisplayName = "Lobby"),
	Title UMETA(DisplayName = "Title")
};
