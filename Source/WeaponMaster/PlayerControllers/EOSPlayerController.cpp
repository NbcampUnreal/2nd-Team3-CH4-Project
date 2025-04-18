#include "EOSPlayerController.h"

#include "GameModes/EOSGameMode.h"
#include "GameState/WeaponMasterGameState.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/WeaponMasterPlayerState.h"
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
    else if (MapName.Contains("VSMap"))
    {
        CurrentMap = EMapType::PVPMap;
    }
    else if (MapName.Contains("PVEMap"))
    {
        CurrentMap = EMapType::PVEMap;
    }

    UE_LOG(LogTemp, Warning, TEXT("Map Name ==> [%d]"), CurrentMap);
    
    if (MapName == "MainSessionMap" || MapName == "VSMap" || MapName == "PVEMap")
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

void AEOSPlayerController::Client_PlayerDead_Implementation(const FString& Killer, const FString& Victim)
{
    if (const ADeathMatchHUD* DeathMatchHUD = Cast<ADeathMatchHUD>(GetHUD()))
    {
        FText Winner = FText::FromString(Killer);
        FText Looser = FText::FromString(Victim);
        
        DeathMatchHUD->UpdateKillLog(Winner, Looser);
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

void AEOSPlayerController::Client_UpdatePlayers_Implementation()
{
    GetWorld()->GetTimerManager().SetTimer(PlayerStatusTimerHandle, this, &AEOSPlayerController::SetPlayerStatus, 1.0f, false);
}

void AEOSPlayerController::SetPlayerStatus()
{
    if (!IsRunningDedicatedServer())
    {
        if (AGameStateBase* GS = GetWorld()->GetGameState())
        {
            for (APlayerState* PS : GS->PlayerArray)
            {
                if (const AWeaponMasterPlayerState* WMPS = Cast<AWeaponMasterPlayerState>(PS))
                {
                    FString PlayerName = WMPS->GetPlayerName();
                    if (const ADeathMatchHUD* DeathMatchHUD = Cast<ADeathMatchHUD>(GetHUD()))
                    {
                        DeathMatchHUD->IndividualMatchStatusWidget->UpdatePlayer(WMPS->GetPlayerId(), PlayerName);
                    }
                }
            }
        }
    }

    GetWorld()->GetTimerManager().ClearTimer(PlayerStatusTimerHandle);
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
                if (AWeaponMasterPlayerState* WMPS = Cast<AWeaponMasterPlayerState>(PlayerState))
                {
                    if (const UWeaponMasterGameInstance* WMGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
                    {
                        const FString PlayerName = WMGI->GetPlayerName();
                        WMPS->SetPlayerName(PlayerName);
                    }
                }
                Server_RegisterPlayer(this);
                break;
            }
        }
    default: break;
    }
}

void AEOSPlayerController::Client_ShowGameResult_Implementation(const TArray<FPlayerResultData>& ResultList)
{
    if (GameResultWidgetClass)
    {
        GameResultWidgetInstance = CreateWidget<UGameResultWidget>(this, GameResultWidgetClass);

        if (GameResultWidgetInstance)
        {
            GameResultWidgetInstance->AddToViewport();
            GameResultWidgetInstance->PopulatePlayerEntries(ResultList);

            UE_LOG(LogTemp, Warning, TEXT("게임 결과 위젯 생성 및 데이터 표시 완료"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("위젯 인스턴스 생성 실패"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameResultWidgetClass가 설정되지 않음"));
    }
}
