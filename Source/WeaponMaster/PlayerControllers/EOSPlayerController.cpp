#include "EOSPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameModes/EOSGameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MultiUI/MultiGameHUD.h"
#include "UI/MultiUI/PlayerNameWidget.h"
#include "UI/MultiUI/SessionLobbyWidget.h"

AEOSPlayerController::AEOSPlayerController()
{
    
}

void AEOSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AEOSPlayerController::BeginPlay"));

    if (UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
    {
        MyGI->OnProcessReturnValue.AddUObject(this, &AEOSPlayerController::HandleProcessResult);
    }

    GetWorldTimerManager().SetTimer(
        HUDTimerHandle,
        this,
        &AEOSPlayerController::HandleTimerAction,
        5.0f,
        false
    );

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);

    SetInputMode(InputMode);
    bShowMouseCursor = true;
}

void AEOSPlayerController::OnStartSessionButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("StartSessionButton clicked!"));
    Server_StartSession();
}

void AEOSPlayerController::OnLoginButtonClicked()
{
    bool bIsPlayerLoggedin = false;;
    if (const UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
    {
        bIsPlayerLoggedin = MyGI->IsPlayerLoggedIn();
    }

    if (!IsRunningDedicatedServer() && !bIsPlayerLoggedin)
    {
        UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance());
        check(MyGI);
        MyGI->Login();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player already logged in!"));
    }
}

void AEOSPlayerController::HandleTimerAction()
{
    UE_LOG(LogTemp, Warning, TEXT("HandleTimerAction"));
    if (!IsRunningDedicatedServer())
    {
        if (const UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
        {
            const FString PlayerName = MyGI->GetPlayerName();
            if (AMultiGameHUD *MultiHUD = Cast<AMultiGameHUD>(GetHUD()))
            {
                UE_LOG(LogTemp, Warning, TEXT("BindAction!!"));
                MultiHUD->SessionLobbyWidget->OnStartSessionClicked.AddDynamic(this, &AEOSPlayerController::OnStartSessionButtonClicked);
                MultiHUD->SessionLobbyWidget->OnLoginClicked.AddDynamic(this, &AEOSPlayerController::OnLoginButtonClicked);
                
                if (MultiHUD->PlayerNameWidget)
                {        
                    MultiHUD->PlayerNameWidget->SetPlayerName(PlayerName);
                    MultiHUD->PlayerNameWidget->PlayerNameText->SetVisibility(ESlateVisibility::Visible);
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("SetPlayerName = [%s]"), *PlayerName);
        }
    }

    GetWorldTimerManager().ClearTimer(HUDTimerHandle);
}

void AEOSPlayerController::OnNetCleanup(class UNetConnection* Connection)
{
    Super::OnNetCleanup(Connection);
}

void AEOSPlayerController::Server_RegisterPlayer_Implementation(APlayerController* PlayerController)
{
    AEOSGameMode* GameMode = Cast<AEOSGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        GameMode->RegisterPlayer(PlayerController);
    }
}

void AEOSPlayerController::Server_StartSession_Implementation()
{
    AEOSGameMode* GameMode = Cast<AEOSGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        GameMode->StartSession();
    }
}

void AEOSPlayerController::HandleProcessResult(EPlayerEOSStateType State, ESessionResultType Result)
{
    UE_LOG(LogTemp, Warning, TEXT("Player Controller Process Result [%d][%d]"), State, Result);
    switch (State)
    {
    case EPlayerEOSStateType::Login:
        {
            if (Result == ESessionResultType::Success)
            {
                break;
            }
        }
    case EPlayerEOSStateType::FindSession:
        break;
    case EPlayerEOSStateType::JoinSession:
        {
            if (Result == ESessionResultType::Success)
            {
                Server_RegisterPlayer(this);
                break;
            }
        }
    default: break;
    }
}
