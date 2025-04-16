// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Notify/ProjectileFire.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UProjectileFire::UProjectileFire()
{
	ProjectileClass = nullptr; // 기본값 없음
	SpawnLocationOffset = FVector(0.0f, 0.0f, 0.0f); 
	bDrawDebug = false;
	DebugLineColor = FLinearColor::Red;
	DebugSphereRadius = 10.0f;
	DebugDuration = 10.0f; 
	DebugSegments = 12;
	DebugThickness = 0.0f;
	
}

void UProjectileFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference); 

	if (!MeshComp || !MeshComp->GetOwner())
	{
		UE_LOG(LogAnimation, Warning, TEXT("UProjectileFire: MeshComp 또는 Owner가 유효하지 않습니다.")); 
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner(); 
	UWorld* World = OwnerActor->GetWorld(); 

	if (!World)
	{
		UE_LOG(LogAnimation, Warning, TEXT("UProjectileFire: OwnerActor로부터 World를 가져올 수 없습니다.")); 
		return;
	}

	const FTransform OwnerTransform = OwnerActor->GetActorTransform();
	const FVector ConversionSpawnLocation = UKismetMathLibrary::TransformLocation(OwnerTransform, SpawnLocationOffset);

	if (bDrawDebug)
	{
		UKismetSystemLibrary::DrawDebugSphere(
			World,                       // 월드 컨텍스트 객체
			ConversionSpawnLocation,     // 중심점 (계산된 월드 위치)
			DebugSphereRadius,           // 반경
			DebugSegments,               // 세그먼트 수
			DebugLineColor,              // 선 색상
			DebugDuration,               // 지속 시간
			DebugThickness               // 두께
		);
	}
	
	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor); 
	
	if (!OwnerCharacter)
	{
		UE_LOG(LogAnimation, Warning, TEXT("UProjectileFire: OwnerActor가 ACharacter가 아닙니다. Owner/Instigator 설정 없이 스폰을 시도합니다. (BP에서는 여기서 멈춤)")); 
		return; 
	}
	
	// 서버에서만 프로젝타일을 스폰하도록 체크
	if (!OwnerCharacter->HasAuthority())
	{
		UE_LOG(LogAnimation, Log, TEXT("UProjectileFire: 클라이언트에서는 프로젝타일을 스폰하지 않습니다.")); 
		return;
	}
	
	if (!ProjectileClass)
	{
		UE_LOG(LogAnimation, Warning, TEXT("UProjectileFire: ProjectileClass가 설정되지 않았습니다!")); 
		return;
	}

	FActorSpawnParameters SpawnParams;
	if (OwnerCharacter)
	{
		SpawnParams.Owner = OwnerCharacter;           
		SpawnParams.Instigator = OwnerCharacter;      
	}
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 

	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, ConversionSpawnLocation, FVector::OneVector);

	AActor* SpawnedActor = World->SpawnActor<AActor>(ProjectileClass, SpawnTransform, SpawnParams);

	if (SpawnedActor)
	{
		UE_LOG(LogAnimation, Log, TEXT("UProjectileFire: 액터 %s (클래스: %s) 스폰 성공"), *SpawnedActor->GetName(), *ProjectileClass->GetName()); 
	}
	else
	{
		UE_LOG(LogAnimation, Error, TEXT("UProjectileFire: 클래스 %s의 액터 스폰 실패!"), *ProjectileClass->GetName()); 
	}

}