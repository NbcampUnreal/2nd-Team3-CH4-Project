#include "EOSPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameModes/EOSGameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MultiUI/MultiGameHUD.h"
#include "UI/MultiUI/PlayerNameWidget.h"
#include "UI/MultiUI/SessionLobbyWidget.h"
#include "UI/MultiUI/SessionWidget.h"

AEOSPlayerController::AEOSPlayerController()
{
    
}

void AEOSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetTimer();

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);

    SetInputMode(InputMode);
    bShowMouseCursor = true;
}

void AEOSPlayerController::SetTimer()
{
    GetWorldTimerManager().SetTimer(
        HUDTimerHandle,
        this,
        &AEOSPlayerController::HandleTimerAction,
        5.0f,
        false
    );

    GetWorldTimerManager().SetTimer(
        PlayCountDownTimerHandle,
        this,
        &AEOSPlayerController::PlayCountDownTimerAction,
        1.0f,
        true
    );
}

void AEOSPlayerController::PlayCountDownTimerAction()
{
    --TimerCountDown;

    if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        MultiGameHUD->MapSelectWidget->SetTimer(TimerCountDown);
    }

    if (TimerCountDown == 0)
    {
        GetWorldTimerManager().ClearTimer(PlayCountDownTimerHandle);
    }
}

void AEOSPlayerController::AddDelegate()
{
    if (UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
    {
        MyGI->OnProcessReturnValue.AddUObject(this, &AEOSPlayerController::HandleProcessResult);
    }

    if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        FString PlayerName = "";
        if (const UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
        {
            PlayerName = MyGI->GetPlayerName();
        }
        
        UE_LOG(LogTemp, Warning, TEXT("SetPlayerName = [%s]"), *PlayerName);
        UE_LOG(LogTemp, Warning, TEXT("BindAction!!"));
        
        MultiGameHUD->MapSelectWidget->OnCooperateButtonClickedDelegate.AddDynamic(this, &AEOSPlayerController::OnCooperateButtonClicked);
        MultiGameHUD->MapSelectWidget->OnDeathMatchButtonClickedDelegate.AddDynamic(this, &AEOSPlayerController::OnDeathMatchButtonClicked);
        
        MultiGameHUD->SessionLobbyWidget->OnStartSessionClicked.AddDynamic(this, &AEOSPlayerController::OnStartSessionButtonClicked);
        MultiGameHUD->SessionLobbyWidget->OnLoginClicked.AddDynamic(this, &AEOSPlayerController::OnLoginButtonClicked);
        
        if (MultiGameHUD->PlayerNameWidget)
        {        
            MultiGameHUD->PlayerNameWidget->SetPlayerName(PlayerName);
            MultiGameHUD->PlayerNameWidget->PlayerNameText->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void AEOSPlayerController::OnCooperateButtonClicked()
{
    
}

void AEOSPlayerController::OnDeathMatchButtonClicked()
{
    
}

void AEOSPlayerController::Client_UpdateTotalPlayerNum_Implementation(int16 PlayerNum)
{
    if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        MultiGameHUD->MapSelectWidget->SetTotalPlayers(PlayerNum);
    }
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
        AddDelegate();
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
