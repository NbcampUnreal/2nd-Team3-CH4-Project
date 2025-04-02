// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 콜리전 컴포넌트 설정
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnOverlapEnd);

	// 메시 컴포넌트 설정
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemBase::OnPickup_Implementation(ACharacter* Character)
{
	// 아이템 획득 사운드 재생
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void AItemBase::OnEquip_Implementation(ACharacter* Character)
{
	// 기본 장착 동작 (하위 클래스에서 재정의)
}

void AItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[AItemBase::OnOverlapBegin] 오버랩됨 %s"), *OtherActor->GetName());
	// 플레이어와 오버랩 시 UI 표시
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AItemBase::OnOverlapBegin] 캐릭터임 %s"), *OtherActor->GetName());
		// UI 표시 함수 호출 (플레이어 컨트롤러나 HUD에 구현)
		//Character->ShowItemPickupPrompt(this);
	}
}

void AItemBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("[AItemBase::OnOverlapBegin] 오버랩끝남 %s"), *OtherActor->GetName());
	// 오버랩 종료 시 UI 숨김
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AItemBase::OnOverlapBegin] 캐릭터임 %s"), *OtherActor->GetName());
		//Character->HideItemPickupPrompt();
	}
}

