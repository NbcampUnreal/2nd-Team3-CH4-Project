#include "EOSPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameModes/EOSGameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MultiUI/SessionLobbyWidget.h"

AEOSPlayerController::AEOSPlayerController()
{
    
}

void AEOSPlayerController::BeginPlay()
{
    UE_LOG(LogTemp, Display, TEXT("AEOSPlayerController::BeginPlay"));
    Super::BeginPlay();

    if (UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
    {
        MyGI->OnProcessReturnValue.AddUObject(this, &AEOSPlayerController::HandleProcessResult);
    }

    if (SessionLobbyWidgetClass)
    {
        SessionLobbyWidget = CreateWidget<USessionLobbyWidget>(GetWorld(), SessionLobbyWidgetClass);
        if (SessionLobbyWidget)
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(SessionLobbyWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputMode.SetHideCursorDuringCapture(false);

            SetInputMode(InputMode);
            bShowMouseCursor = true;
            
            SessionLobbyWidget->AddToViewport();
            SessionLobbyWidget->OnStartSessionClicked.AddDynamic(this, &AEOSPlayerController::OnStartSessionButtonClicked);
            SessionLobbyWidget->OnLoginClicked.AddDynamic(this, &AEOSPlayerController::OnLoginButtonClicked);
        }
    }
}

void AEOSPlayerController::OnStartSessionButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("StartSessionButton clicked!"));
    Server_StartSession();
}

void AEOSPlayerController::OnLoginButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("StartSessionButton clicked!"));

    if (!IsRunningDedicatedServer())
    {
        UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance());
        check(MyGI);
        MyGI->Login();
    }
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

void AEOSPlayerController::HandleProcessResult(EMyStateType State, EMyResultType Result)
{
    UE_LOG(LogTemp, Warning, TEXT("Player Controller Process Result [%d][%d]"), State, Result);
    switch (State)
    {
    case EMyStateType::Login:
        {
            if (Result == EMyResultType::Success)
            {
                break;
            }
        }
    case EMyStateType::FindSession:
        break;
    case EMyStateType::JoinSession:
        {
            if (Result == EMyResultType::Success)
            {
                Server_RegisterPlayer(this);
                break;
            }
        }
    default: break;
    }
}
