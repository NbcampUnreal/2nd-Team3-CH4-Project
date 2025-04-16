#include "WeaponMasterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AICharacter/BossCharacter.h"

AWeaponMasterAIController::AWeaponMasterAIController()
{
	// Perception 컴포넌트 생성
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	// 시야 감각 구성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 10000.f;
	SightConfig->LoseSightRadius = 12000.f;
	SightConfig->PeripheralVisionAngleDegrees = 360.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	// 델리게이트 바인딩
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AWeaponMasterAIController::OnTargetPerceptionUpdated);
}

void AWeaponMasterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
	else
	{
	}
}


void AWeaponMasterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	EvaluateTargetPriority();
}

void AWeaponMasterAIController::EvaluateTargetPriority()
{
	if (!IsValid(GetPawn())) return;

	ABossCharacter* AIChar = Cast<ABossCharacter>(GetPawn());
	if (!AIChar || AIChar->IsAttacking())
	{
		return;
	}

	TArray<AActor*> PerceivedActors;
	AIPerception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	AActor* BestTarget = nullptr;
	float ClosestDist = FLT_MAX;

	if (APawn* MyPawn = GetPawn())
	{
		const FVector MyLocation = MyPawn->GetActorLocation();

		for (AActor* Target : PerceivedActors)
		{
			if (!IsValid(Target)) continue;

			const float Dist = FVector::Dist(MyLocation, Target->GetActorLocation());
			if (Dist < ClosestDist)
			{
				BestTarget = Target;
				ClosestDist = Dist;
			}
		}
	}

	UObject* CurrentTarget = GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor"));
	if (BestTarget && CurrentTarget != BestTarget)
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), BestTarget);
	}
}