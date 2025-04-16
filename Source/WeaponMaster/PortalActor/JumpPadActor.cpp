#include "JumpPadActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

AJumpPadActor::AJumpPadActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SetRootComponent(RootComp);

    JumpTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("JumpTrigger"));
    JumpTrigger->SetupAttachment(RootComp);
    JumpTrigger->SetCollisionProfileName(TEXT("Trigger"));
    JumpTrigger->SetBoxExtent(FVector(50.0f, 50.0f, 20.0f));

    JumpPadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JumpPadMesh"));
    JumpPadMesh->SetupAttachment(RootComp);

    JumpPadMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JumpPadMesh2"));
    JumpPadMesh2->SetupAttachment(RootComp);

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(RootComponent);

    DestinationLocation = CreateDefaultSubobject<USceneComponent>(TEXT("DestinationLocation"));
    DestinationLocation->SetupAttachment(RootComp);
    DestinationLocation->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));

    // 기본값 설정
    bIsJumpPadActive = true;
    JumpForce = 2000.f;
    AnimationDuration = 3.0f;
    JumpPadAnimHeight = 1000.0f;
    TransportSpeed = 3000.f;
    DestinationOffset = 300.0f;
    bIsAnimating = false;
    CurrentTransportingCharacter = nullptr;
    TransportUpdateInterval = 0.03f; 
}

void AJumpPadActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AJumpPadActor, DestinationJumpPad);
    DOREPLIFETIME(AJumpPadActor, bIsJumpPadActive);
    DOREPLIFETIME(AJumpPadActor, bIsAnimating);
}

void AJumpPadActor::BeginPlay()
{
    Super::BeginPlay();

    JumpTrigger->OnComponentBeginOverlap.AddDynamic(this, &AJumpPadActor::OnJumpPadBeginOverlap);
    JumpTrigger->OnComponentEndOverlap.AddDynamic(this, &AJumpPadActor::OnJumpPadEndOverlap);

    // 초기 위치 저장 (애니메이션용)
    OriginalJumpPadLocation = JumpPadMesh->GetRelativeLocation();

    UpdateJumpPadMaterial();
}
bool AJumpPadActor::IsTransporting() const
{
    return GetWorldTimerManager().IsTimerActive(TransportTimerHandle) && CurrentTransportingCharacter != nullptr;
}
// 타이머 시작 메서드
void AJumpPadActor::StartTransportTimer(ACharacter* Character)
{
    if (!Character || !HasAuthority())
    {
        return;
    }

    CurrentTransportingCharacter = Character;

    // 타이머 시작
    GetWorldTimerManager().SetTimer(
        TransportTimerHandle,
        this,
        &AJumpPadActor::OnTransportTimerTick,
        TransportUpdateInterval,
        true);
}

// 타이머 종료 메서드
void AJumpPadActor::StopTransportTimer()
{
    if (HasAuthority())
    {
        GetWorldTimerManager().ClearTimer(TransportTimerHandle);

        // 점프 패드 활성화
        SetJumpPadActive(true);

        // 목적지 점프패드도 활성화
        if (DestinationJumpPad)
        {
            DestinationJumpPad->SetJumpPadActive(true);
        }

        if (CurrentTransportingCharacter)
        {
            CurrentTransportingCharacter = nullptr;
        }
        Multicast_EndJumpEffect();
    }
}

void AJumpPadActor::SetJumpPadActive(bool bActive)
{
    if (HasAuthority())
    {
        bIsJumpPadActive = bActive;
    }
}

void AJumpPadActor::OnRep_JumpPadActive()
{
    UpdateJumpPadMaterial();
}

void AJumpPadActor::UpdateJumpPadMaterial()
{
	if (JumpPadMesh && DestinationJumpPad->JumpPadMesh)
    {
        UMaterialInterface* MatToApply = bIsJumpPadActive ? ActiveMaterial : InactiveMaterial;
        if (MatToApply)
        {
            JumpPadMesh->SetMaterial(0, MatToApply);
            DestinationJumpPad->JumpPadMesh->SetMaterial(0, MatToApply);
        }
    }
}

bool AJumpPadActor::IsDestinationClear(ACharacter* Character)
{
    if (!DestinationJumpPad || !Character)
    {
        return false;
    }

    FVector DestLocation = DestinationJumpPad->DestinationLocation->GetComponentLocation();
    FQuat DestRotation = DestinationJumpPad->DestinationLocation->GetComponentQuat();

    FVector Offset = DestRotation.GetForwardVector() * DestinationOffset;
    DestLocation += Offset;

    UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
    if (!CapsuleComp)
    {
        return false;
    }

    FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(
        CapsuleComp->GetScaledCapsuleRadius(),
        CapsuleComp->GetScaledCapsuleHalfHeight());

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(DestinationJumpPad);
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

void AJumpPadActor::OnJumpPadBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character || !bIsJumpPadActive)
    {
        return;
    }

    if (!HasAuthority())
    {
        return;
    }
    if (CurrentTransportingCharacter)
    {
        return;
    }
    Server_LaunchCharacter(Character);
}

void AJumpPadActor::OnJumpPadEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
    {
        return;
    }

    // 서버에서만 처리
    if (HasAuthority())
    {
        if (!IsTransporting())
        {
            SetJumpPadActive(true);

            // 목적지가 있으면 목적지 점프 패드도 활성화
            if (DestinationJumpPad)
            {
                DestinationJumpPad->SetJumpPadActive(true);
            }
        }
    }
}

bool AJumpPadActor::Server_LaunchCharacter_Validate(ACharacter* Character)
{
    return Character != nullptr;
}

void AJumpPadActor::Server_LaunchCharacter_Implementation(ACharacter* Character)
{
    if (!Character || !HasAuthority() || !bIsJumpPadActive || IsTransporting())
    {
        return;
    }

    // 점프 및 이동 실행
    if (ExecuteJumpAndTransport(Character))
    {
        // 성공 시 효과 트리거
        Multicast_OnJumpEffect();
    }
}

bool AJumpPadActor::ExecuteJumpAndTransport(ACharacter* Character)
{
    if (!Character || !HasAuthority())
    {
        return false;
    }
    // 이미 전송 중인지 확인
    if (IsTransporting())
    {
        return false;
    }

    // 목적지가 있으면 목적지가 비어있는지 확인
    if (DestinationJumpPad && !IsDestinationClear(Character))
    {
        return false;
    }
   

    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp)
    {
        //MovementComp->AddImpulse(FVector(0, 0, JumpForce), true);
        Character->LaunchCharacter(FVector(0, 0, JumpForce), false, true);
    }

    if (DestinationJumpPad)
    {
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this, Character]()
            {
                StartCharacterTransport(Character);
            }, 0.5f, false);
    }
    SetJumpPadActive(false);
    if (DestinationJumpPad)
    {
        DestinationJumpPad->SetJumpPadActive(false);
    }
    Multicast_PlayJumpPadAnimation();
    return true;
}
void AJumpPadActor::OnTransportTimerTick()
{
    if (!CurrentTransportingCharacter || !DestinationJumpPad || !HasAuthority())
    {
        StopTransportTimer();
        return;
    }
    FVector CurrentLocation = CurrentTransportingCharacter->GetActorLocation();
 
    FVector TargetLocation = DestinationJumpPad->DestinationLocation->GetComponentLocation();
    FVector ForwardDir = DestinationJumpPad->DestinationLocation->GetForwardVector();
    TargetLocation += ForwardDir * DestinationOffset;

    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

    float MoveDistance = TransportSpeed * TransportUpdateInterval;
    FVector NewLocation = CurrentLocation + Direction * MoveDistance;


    float DistanceToTarget = FVector::Distance(CurrentLocation, TargetLocation);
    if (DistanceToTarget <= MoveDistance * 1.5f)
    {
        // 크래시 방지를 위해 여기서 목적지 위치로 설정하지 않고 정지
        CompleteTransport();
        return;
    }

    // 캐릭터 위치 업데이트 (안전하게)
    if (IsValid(CurrentTransportingCharacter))
    {
        CurrentTransportingCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
    }
    else
    {
        StopTransportTimer();
    }
}
void AJumpPadActor::StartCharacterTransport(ACharacter* Character)
{
    if (!Character || !DestinationJumpPad)
    {
        return;
    }

    // 캐릭터 무브먼트 비활성화
    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp)
    {
        MovementComp->SetMovementMode(MOVE_Flying);
        MovementComp->DisableMovement();
    }

    StartTransportTimer(Character);
}

void AJumpPadActor::CompleteTransport()
{
    if (!CurrentTransportingCharacter)
    {
        StopTransportTimer();
        return;
    }
    GetWorldTimerManager().ClearTimer(TransportTimerHandle);

    if (DestinationJumpPad)
    {
        FTransform DestinationTransform = DestinationJumpPad->DestinationLocation->GetComponentTransform();


        FVector SafeLocation = DestinationJumpPad->DestinationLocation->GetComponentLocation();
        SafeLocation += DestinationJumpPad->DestinationLocation->GetForwardVector() * DestinationOffset;

        CurrentTransportingCharacter->SetActorLocation(SafeLocation, false, nullptr, ETeleportType::TeleportPhysics);

        OrientCharacterAfterTransport(CurrentTransportingCharacter, DestinationTransform);
    }

    // 캐릭터 무브먼트 다시 활성화
    UCharacterMovementComponent* MovementComp = CurrentTransportingCharacter->GetCharacterMovement();
    if (MovementComp)
    {
        MovementComp->SetDefaultMovementMode();
        MovementComp->SetMovementMode(MOVE_Falling);
    }

    ACharacter* CompletedCharacter = CurrentTransportingCharacter;
    CurrentTransportingCharacter = nullptr;

    SetJumpPadActive(true);
    if (DestinationJumpPad)
    {
        DestinationJumpPad->SetJumpPadActive(true);
    }
    Multicast_EndJumpEffect();
}
void AJumpPadActor::TransportCharacter(float DeltaTime)
{
    if (!CurrentTransportingCharacter || !DestinationJumpPad)
    {
        return;
    }


    FVector CurrentLocation = CurrentTransportingCharacter->GetActorLocation();
    FVector TargetLocation = DestinationJumpPad->DestinationLocation->GetComponentLocation();
    FVector ForwardDir = DestinationJumpPad->DestinationLocation->GetForwardVector();
    TargetLocation += ForwardDir * DestinationOffset;

    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

    float MoveDistance = TransportSpeed * DeltaTime;
    FVector NewLocation = CurrentLocation + Direction * MoveDistance;

    float DistanceToTarget = FVector::Distance(CurrentLocation, TargetLocation);
    if (DistanceToTarget <= MoveDistance * 1.5f)
    {

        NewLocation = TargetLocation;
        CompleteTransport();
    }
    CurrentTransportingCharacter->SetActorLocation(NewLocation);
}
void AJumpPadActor::OrientCharacterAfterTransport(ACharacter* Character, const FTransform& DestinationTransform)
{
    if (!Character || !HasAuthority())
    {
        return;
    }

    FRotator NewRotation = DestinationTransform.GetRotation().Rotator();

    if (Character->GetController())
    {
        Character->GetController()->SetControlRotation(NewRotation);
    }

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

void AJumpPadActor::Multicast_PlayJumpPadAnimation_Implementation()
{
    if (bIsAnimating)
        return;
    bIsAnimating = true;
    OriginalJumpPadLocation = JumpPadMesh->GetRelativeLocation(); // 현재 위치를 기준으로 설정

    // 애니메이션 시작 시간 저장
    AnimationStartTime = GetWorld()->GetTimeSeconds();

    GetWorldTimerManager().SetTimer(
        AnimationUpdateTimerHandle,
        this,
        &AJumpPadActor::UpdateJumpPadAnimation,
        0.016f,
        true);
    
    UpdateJumpPadMaterial();

}
void AJumpPadActor::UpdateJumpPadAnimation()
{
    if (!bIsAnimating)
        return;


    float ElapsedTime = GetWorld()->GetTimeSeconds() - AnimationStartTime;
    float Alpha = FMath::Clamp(ElapsedTime / AnimationDuration, 0.0f, 1.0f);
    float HeightOffset = JumpPadAnimHeight * FMath::Sin(Alpha * PI);

    JumpPadMesh->SetRelativeLocation(OriginalJumpPadLocation + FVector(0, 0, HeightOffset));

    if (Alpha >= 1.0f)
    {
        CompleteJumpPadAnimation();
    }
}
void AJumpPadActor::OnRep_IsAnimating()
{
    // 애니메이션 상태가 바뀔 때 실행할 로직
    if (bIsAnimating)
    {
        //// 애니메이션이 시작될 때
        //OriginalJumpPadLocation = JumpPadMesh->GetRelativeLocation();
        //AnimationStartTime = GetWorld()->GetTimeSeconds();

        //// 애니메이션 업데이트 타이머 시작
        //GetWorldTimerManager().SetTimer(
        //    AnimationUpdateTimerHandle,
        //    this,
        //    &AJumpPadActor::UpdateJumpPadAnimation,
        //    0.016f,
        //    true);
    }
    else
    {
        // 애니메이션이 종료될 때
        GetWorldTimerManager().ClearTimer(AnimationUpdateTimerHandle);
        JumpPadMesh->SetRelativeLocation(OriginalJumpPadLocation);
    }
}
void AJumpPadActor::CompleteJumpPadAnimation()
{
    GetWorldTimerManager().ClearTimer(AnimationUpdateTimerHandle);
    JumpPadMesh->SetRelativeLocation(OriginalJumpPadLocation);
    bIsAnimating = false;
}

void AJumpPadActor::Multicast_OnJumpEffect_Implementation()
{
    FVector EffectLocation = GetActorLocation();

    if (NiagaraComponent)
    {
        NiagaraComponent->Activate(true);
        DestinationJumpPad->NiagaraComponent->Activate(true);
    }

    if (JumpSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            JumpSound,
            EffectLocation,
            1.0f,
            1.0f,
            0.0f);
    }
}
void AJumpPadActor::Multicast_EndJumpEffect_Implementation()
{
    UpdateJumpPadMaterial();
}