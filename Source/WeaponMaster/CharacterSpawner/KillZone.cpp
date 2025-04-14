#include "KillZone.h"

AKillZone::AKillZone()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKillZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKillZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

