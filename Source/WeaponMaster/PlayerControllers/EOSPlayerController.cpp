#include "EOSPlayerController.h"

#include "GameModes/EOSGameMode.h"
#include "GameState/WeaponMasterGameState.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MultiUI/DeathMatchHUD.h"
#include "UI/MultiUI/IndividualMatchStatusWidget.h"
#include "UI/MultiUI/MultiGameHUD.h"
#include "UI/MultiUI/SessionWidget.h"
#include "UI/MultiUI/WrapStatusWidget.h"

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

    FString MapName = FPackageName::GetShortName(GetWorld()->GetMapName());
    
    if (MapName.Contains("MainSessionMap"))
    {
        CurrentMap = EMapType::SessionMap;
    }
    else if (MapName.Contains("PVPMap"))
    {
        CurrentMap = EMapType::PVPMap;
    }
    else if (MapName.Contains("PVEMap"))
    {
        CurrentMap = EMapType::PVEMap;
    }

    UE_LOG(LogTemp, Warning, TEXT("Map Name ==> [%d]"), CurrentMap);
    
    if (MapName == "MainSessionMap" || MapName == "PVPMap" || MapName == "PVEMap")
    {
        Login();
    }
}

void AEOSPlayerController::Client_UpdateInGameTimer_Implementation(const int32 TimerCountDown)
{
    if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        MultiGameHUD->WrapStatusWidget->SetRemainTimer(TimerCountDown);
    }

    if (ADeathMatchHUD* DeathMatchHUD = Cast<ADeathMatchHUD>(GetHUD()))
    {
        DeathMatchHUD->SetRemainingTime(TimerCountDown);
    }
}

void AEOSPlayerController::UpdateHUD(EMapType Map)
{
    if (AMultiGameHUD* MultiHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        MultiHUD->TransferHUDBy(Map);
    }

    if (const ADeathMatchHUD* DeathMatchHUD = Cast<ADeathMatchHUD>(GetHUD()))
    {
        DeathMatchHUD->IndividualMatchStatusWidget->UpdateMatchTitle(Map);
        FText Winner = FText::FromString(TEXT("황찬호"));
        FText Looser = FText::FromString(TEXT("재성윤"));
        
        DeathMatchHUD->UpdateKillLog(Winner, Looser);
    }
}

void AEOSPlayerController::Client_UpdateTimer_Implementation(const int32 TimerCountDown)
{
    if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        MultiGameHUD->MapSelectWidget->SetTimer(TimerCountDown);
    }
}

void AEOSPlayerController::SetTimer()
{
    GetWorldTimerManager().SetTimer(
        HUDTimerHandle,
        this,
        &AEOSPlayerController::HandleTimerAction,
        2.0f,
        false
    );
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
    }
}

void AEOSPlayerController::OnCooperateButtonClicked()
{
    if (bIsCooperateVoted) return;
    Server_SetCooperationMapSelected(bIsVoted);
    SetSelectedPlayerWidget();
    bIsVoted = true;
    bIsCooperateVoted = true;
    bIsDeathMatchVoted = false;
}

void AEOSPlayerController::Server_SetCooperationMapSelected_Implementation(bool IsVoted)
{
    if (AWeaponMasterGameState* GameState = GetWorld()->GetGameState<AWeaponMasterGameState>())
    {
        GameState->SetCooperateVotedPlayerNum(IsVoted);
    }
}

void AEOSPlayerController::OnDeathMatchButtonClicked()
{
    if (bIsDeathMatchVoted) return;
    Server_SetDeathMatchMapSelected(bIsVoted);
    SetSelectedPlayerWidget();
    bIsVoted = true;
    bIsCooperateVoted = false;
    bIsDeathMatchVoted = true;
}

void AEOSPlayerController::Server_SetDeathMatchMapSelected_Implementation(bool IsVoted)
{
    if (AWeaponMasterGameState* GameState = GetWorld()->GetGameState<AWeaponMasterGameState>())
    {
        GameState->SetDeathMatchVotedPlayerNum(IsVoted);
    }
}

void AEOSPlayerController::SetSelectedPlayerWidget()
{
    if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        if (const AWeaponMasterGameState* GameState = GetWorld()->GetGameState<AWeaponMasterGameState>())
        {
            MultiGameHUD->MapSelectWidget->SetCooperateMapSelectedPlayers(GameState->GetCooperateVotedPlayerNum());
            MultiGameHUD->MapSelectWidget->SetDeathMatchMapSelectedPlayers(GameState->GetDeathMatchVotedPlayerNum());
        }
    }
}

void AEOSPlayerController::Client_UpdateTotalPlayerNum_Implementation(int16 PlayerNum)
{
    if (const AMultiGameHUD* MultiGameHUD = Cast<AMultiGameHUD>(GetHUD()))
    {
        MultiGameHUD->MapSelectWidget->SetTotalPlayers(PlayerNum);
    }
}

void AEOSPlayerController::Login()
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
        UpdateHUD(CurrentMap);
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
