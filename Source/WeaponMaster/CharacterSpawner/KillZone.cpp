#include "KillZone.h"

#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "Components/BoxComponent.h"

AKillZone::AKillZone()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AKillZone::OnOverlapBegin);
}

void AKillZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKillZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AKillZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							   bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AKillZone::OnOverlapBegin"));
	if (ABaseBattleCharacter* BaseBattleCharacter = Cast<ABaseBattleCharacter>(OtherActor))
	{
		BaseBattleCharacter->OnAttacked(
			{
			this,
				100,
				{0, 0, 0},
				{},
				{},
				{},
				{}
			});
	}
}