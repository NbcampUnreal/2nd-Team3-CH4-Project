#include "WeaponMasterController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameState/WeaponMasterGameState.h"
#include "PlayerState/WeaponMasterPlayerState.h"
#include "UI/MultiUI/ChatWidget.h"
#include "UI/MultiUI/DeathMatchHUD.h"
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
	ADeathMatchHUD* DMHUD = Cast<ADeathMatchHUD>(GetHUD());
	if (!DMHUD || !IsValid(DMHUD->ChatWidget)) return;

	if (!bIsChatBound)
	{
		DMHUD->ChatWidget->OnChatMessageCommitted.AddDynamic(this, &AWeaponMasterController::HandleChatMessage);
		bIsChatBound = true;
	}

	if (DMHUD->ChatWidget->IsInViewport())
	{
		const ESlateVisibility CurrentVisibility = DMHUD->ChatWidget->GetVisibility();
		DMHUD->ChatWidget->SetVisibility(
			(CurrentVisibility == ESlateVisibility::Hidden) ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
	else
	{
		DMHUD->ChatWidget->AddToViewport();
		DMHUD->ChatWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

bool AWeaponMasterController::IsChatWidgetValid() const
{
	if (const ADeathMatchHUD* DMHUD = Cast<ADeathMatchHUD>(GetHUD()))
	{
		return IsValid(DMHUD->ChatWidget) && DMHUD->ChatWidget->IsInViewport();
	}
	return false;
}

void AWeaponMasterController::HandleChatMessage(const FText& Message)
{
	FString PlayerName = TEXT("Unknown");
	if (AWeaponMasterPlayerState* WMPS = GetPlayerState<AWeaponMasterPlayerState>())
	{
		PlayerName = WMPS->GetPlayerName();
	}

	const FText FormattedMessage = FText::FromString(FString::Printf(TEXT("%s: %s"), *PlayerName, *Message.ToString()));
	Server_SendChatMessage(FormattedMessage);
}

void AWeaponMasterController::Server_SendChatMessage_Implementation(const FText& Message)
{
	UE_LOG(LogTemp, Display, TEXT("[서버] 클라로부터 받은 메시지: %s"), *Message.ToString());

	if (AWeaponMasterGameState* GS = GetWorld()->GetGameState<AWeaponMasterGameState>())
	{
		GS->Multicast_ChatMessage(Message);
	}
}

