#include "WeaponMasterController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/MultiUI/ChatWidget.h"
#include "UI/MultiUI/MultiGameHUD.h"

AWeaponMasterController::AWeaponMasterController()
{
	DefaultMappingContext = nullptr;
	MoveAction = nullptr;
	CrouchDropAction= nullptr;
	WeakAttackAction = nullptr;
	StrongAttackAction = nullptr;
	IdentityAction = nullptr;
	DefenceAction = nullptr;
	DashAction = nullptr;
	PickingItemAction = nullptr;
	MenuOnOffAction = nullptr;
	ChatAction = nullptr;
}

void AWeaponMasterController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("Player Pointer is null"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(Subsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("Player Subsystem is null"));
		return;
	}
	
	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is null"));
		return;
	}
	
	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void AWeaponMasterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(ChatAction, ETriggerEvent::Started,
										   this, &AWeaponMasterController::Chat);
}

void AWeaponMasterController::SetCurrentCharacterAtGI_Implementation(TSubclassOf<ACharacter> CurrentCharacterClass)
{
	if (auto CastedGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
	{
		CastedGI->CharacterClass = CurrentCharacterClass;
	}
}

void AWeaponMasterController::Chat()
{
	if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
	{
		if (MultiGameHUD->ChatWidget->GetVisibility() == ESlateVisibility::Hidden)
		{
			MultiGameHUD->ChatWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			MultiGameHUD->ChatWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
