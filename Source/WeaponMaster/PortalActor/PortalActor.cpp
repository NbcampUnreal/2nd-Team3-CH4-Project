#include "PortalActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

APortalActor::APortalActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
    PortalMesh->SetupAttachment(RootComponent);

    PortalComponentMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalComponentMesh2"));
    PortalComponentMesh2->SetupAttachment(RootComponent);

    PortalComponentMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalComponentMesh3"));
    PortalComponentMesh3->SetupAttachment(RootComponent);

    PortalComponentMesh4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalComponentMesh4"));
    PortalComponentMesh4->SetupAttachment(RootComponent);

    PortalComponentMesh5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalComponentMesh5"));
    PortalComponentMesh5->SetupAttachment(RootComponent);

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(RootComponent);

    PortalTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PortalTrigger"));
    PortalTrigger->SetupAttachment(RootComponent);
    PortalTrigger->SetCollisionProfileName(TEXT("Trigger"));
    PortalTrigger->SetBoxExtent(FVector(50.0f, 50.0f, 100.0f));

    TeleportLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportLocation"));
    TeleportLocation->SetupAttachment(RootComponent);
    TeleportLocation->SetRelativeLocation(FVector(50.f, 0.0f, 0.0f)); 
}

void APortalActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APortalActor, DestinationPortal);
    DOREPLIFETIME(APortalActor, bIsPortalActive);
}

void APortalActor::BeginPlay()
{
    Super::BeginPlay();

    // 모든 클라이언트에서 시각적 피드백을 위해 서버가 아닌 서버 + 클라이언트로 바인딩
    PortalTrigger->OnComponentBeginOverlap.AddDynamic(this, &APortalActor::OnPortalBeginOverlap);
    PortalTrigger->OnComponentEndOverlap.AddDynamic(this, &APortalActor::OnPortalEndOverlap);
}



void APortalActor::SetPortalActive(bool bActive)
{
    
    if (HasAuthority())
    {
        bIsPortalActive = bActive;
    }
    else return;
}

bool APortalActor::IsDestinationClear(ACharacter* Character)
{
    if (!DestinationPortal || !Character)
    {
        return false;
    }

    // 목적지 주변 충돌 검사
    FVector DestLocation = DestinationPortal->TeleportLocation->GetComponentLocation();
    FQuat DestRotation = DestinationPortal->TeleportLocation->GetComponentQuat();
    FVector Offset = DestRotation.GetForwardVector() * 300.f;
    DestLocation += Offset;

    UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
    if (!CapsuleComp)
    {
        return false;
    }

    // 충돌 검사를 위한 파라미터 설정
    FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(
        CapsuleComp->GetScaledCapsuleRadius(),
        CapsuleComp->GetScaledCapsuleHalfHeight());

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(DestinationPortal);
    QueryParams.AddIgnoredActor(Character);

    // 충돌 검사 수행
    bool bHasCollision = GetWorld()->OverlapAnyTestByProfile(
        DestLocation,
        DestRotation,
        FName("Pawn"),
        CollisionShape,
        QueryParams);

    return !bHasCollision;
}

void APortalActor::OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character || !DestinationPortal)
    {
        return;
    }

    if (!bIsPortalActive)
    {
        return;
    }

    if (!HasAuthority())
    {
        return;
    }

    // 텔레포트 요청 실행 (서버 함수 호출)
    Server_TeleportCharacter(Character);
}

void APortalActor::OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
    {
        return;
    }

    if (HasAuthority())
    {
        SetPortalActive(true);
        DestinationPortal->SetPortalActive(true);
    }
}

bool APortalActor::Server_TeleportCharacter_Validate(ACharacter* Character)
{
    return Character != nullptr && DestinationPortal != nullptr;
}

void APortalActor::Server_TeleportCharacter_Implementation(ACharacter* Character)
{
    if (!Character || !DestinationPortal || !HasAuthority() || !bIsPortalActive)
    {
        return;
    }
    SetPortalActive(false);
    DestinationPortal->SetPortalActive(false);

    if (ExecuteTeleport(Character))
    {
        SetPortalActive(true);
        DestinationPortal->SetPortalActive(true);
    }
}

bool APortalActor::ExecuteTeleport(ACharacter* Character)
{
    if (!DestinationPortal || !Character || !HasAuthority())
    {
        return false;
    }

    if (!IsDestinationClear(Character))
    {
        return false;
    }
    Multicast_OnTeleportEffect();

    FTransform TeleportTransform = DestinationPortal->TeleportLocation->GetComponentTransform();
    FVector Offset = TeleportTransform.GetRotation().GetForwardVector() * 300.f;
    bool bTeleported = Character->TeleportTo(TeleportTransform.GetLocation() + Offset, TeleportTransform.GetRotation().Rotator());

    if (bTeleported)
    {
        OrientCharacterAfterTeleport(Character, TeleportTransform);
    }

    return bTeleported;
}

void APortalActor::OrientCharacterAfterTeleport(ACharacter* Character, const FTransform& DestinationTransform)
{
    if (!Character || !HasAuthority())
    {
        return;
    }

    FRotator NewRotation = DestinationTransform.GetRotation().Rotator();
    Character->GetController()->SetControlRotation(NewRotation);

    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp)
    {
        float CurrentSpeed = MovementComp->Velocity.Size();

        if (CurrentSpeed > 0)
        {
            FVector NewDirection = DestinationTransform.GetRotation().GetForwardVector();
            MovementComp->Velocity = NewDirection * CurrentSpeed;
        }
    }
}


void APortalActor::Multicast_OnTeleportEffect_Implementation()
{
    FVector EffectLocation = GetActorLocation();
    if (NiagaraComponent)
    {
        NiagaraComponent->Activate(true);
        DestinationPortal->NiagaraComponent->Activate(true);
    }
    if (TeleportSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), TeleportSound, EffectLocation);
    }

}

void APortalActor::OnRep_PortalActive()
{
    UpdatePortalMaterial();
}

void APortalActor::UpdatePortalMaterial()
{
    if (PortalMesh)
    {
        UMaterialInterface* MatToApply = bIsPortalActive ? ActiveMaterial : InactiveMaterial;
        if (MatToApply)
        {
            PortalMesh->SetMaterial(0, MatToApply);
        }
    }
}
