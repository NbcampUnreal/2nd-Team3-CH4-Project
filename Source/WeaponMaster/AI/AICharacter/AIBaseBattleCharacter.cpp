// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter/AIBaseBattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponMaster/Data/ItemDataAsset.h"
#include "SSTCharacterMovementComponent.h"

AAIBaseBattleCharacter::AAIBaseBattleCharacter(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
	UE_LOG(LogTemp, Warning, TEXT("[AI] 생성자 호출"));

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;

	UE_LOG(LogTemp, Warning, TEXT("[BaseAI] OrientToMove: %d | UseYaw: %d | YawRate: %f"),
		GetCharacterMovement()->bOrientRotationToMovement,
		bUseControllerRotationYaw,
		GetCharacterMovement()->RotationRate.Yaw);

	if (USSTCharacterMovementComponent* MoveComp = Cast<USSTCharacterMovementComponent>(GetCharacterMovement()))
	{
		MoveComp->SetFacingRight(false); // or true
	}
}

void AAIBaseBattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	RandomItemEquip();
	UE_LOG(LogTemp, Warning, TEXT("[AI] 랜덤 아이템 장착 호출"));
}

void AAIBaseBattleCharacter::RandomItemEquip()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] 랜덤 아이템 장착 하는중"));
	if (ItemAssets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemAssets 배열이 비어 있습니다."));
	}
	int32 RandIndex = FMath::RandRange(0, ItemAssets.Num() - 1);
	UItemDataAsset* SelectedItem = ItemAssets[RandIndex];

	if (SelectedItem)
	{
		FName ItemID = SelectedItem->ItemID;
		EquipItem_Implementation(ItemID);
		UE_LOG(LogTemp, Warning, TEXT("랜덤 장착 성공: %s"), *ItemID.ToString());
	}
}


void AAIBaseBattleCharacter::ApplyWeakAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] ApplyWeakAttack 호출"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 0);
	/*FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = CurrentRotation + FRotator(0.f, 180.f, 0.f);
	SetActorRotation(NewRotation);*/
}

void AAIBaseBattleCharacter::ApplyStrongAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] ApplyStrongAttack 호출"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 1);
}

void AAIBaseBattleCharacter::ApplyIdentity()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] ApplyIdentity 호출"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 2);
}

void AAIBaseBattleCharacter::LookAtTarget(const AActor* TargetActor)
{
	if (!TargetActor) return;

	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.f; // 수평 회전만

	if (!ToTarget.IsNearlyZero())
	{
		FRotator LookAtRotation = ToTarget.Rotation();
		SetActorRotation(LookAtRotation);
	}
}