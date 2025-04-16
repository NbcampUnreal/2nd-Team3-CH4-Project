// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_UpdateTarget.h"
#include "AI/AIController/WeaponMasterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"


UBTService_UpdateTarget::UBTService_UpdateTarget()
{
    NodeName = TEXT("Update TargetActor");
    bNotifyBecomeRelevant = true;
    bNotifyTick = true;
    Interval = 0.5f;
    RandomDeviation = 0.f;
}

void UBTService_UpdateTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AWeaponMasterAIController* MyController = Cast<AWeaponMasterAIController>(OwnerComp.GetAIOwner());
    if (MyController)
    {
        MyController->EvaluateTargetPriority();
    }
}