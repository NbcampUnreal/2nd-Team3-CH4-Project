#include "EOSPlayerController.h"

#include "Instance/WeaponMasterGameInstance.h"

AEOSPlayerController::AEOSPlayerController()
{
    
}

void AEOSPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
}

void AEOSPlayerController::BeginLoginProcess()
{
    if (UWeaponMasterGameInstance* MyGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
    {
        MyGI->Login();
    }
}
