#include "WeaponMasterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

AWeaponMasterAIController::AWeaponMasterAIController()
{
	// Perception 컴포넌트 생성
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	// 시야 감각 구성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	// 델리게이트 바인딩
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AWeaponMasterAIController::OnTargetPerceptionUpdated);
}

void AWeaponMasterAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
		UE_LOG(LogTemp, Warning, TEXT("Behavior Tree started"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BehaviorTreeAsset is NULL!"));
	}
}

void AWeaponMasterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Warning, TEXT("OKOK"))
	if (Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
	}
	else
	{
		GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
	}
}
