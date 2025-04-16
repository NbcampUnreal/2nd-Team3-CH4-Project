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

    DestinationLocation = CreateDefaultSubobject<USceneComponent>(TEXT("DestinationLocation"));
    DestinationLocation->SetupAttachment(RootComp);
    DestinationLocation->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));

    // 기본값 설정
    bIsJumpPadActive = true;
    CooldownTime = 3.0f;
    JumpForce = 2000.f;
    AnimationDuration = 0.5f;
    JumpPadAnimHeight = 2000.0f;
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
    DOREPLIFETIME(AJumpPadActor, CooldownTime);
    DOREPLIFETIME(AJumpPadActor, bIsAnimating);
}

void AJumpPadActor::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 이벤트 바인딩
    JumpTrigger->OnComponentBeginOverlap.AddDynamic(this, &AJumpPadActor::OnJumpPadBeginOverlap);
    JumpTrigger->OnComponentEndOverlap.AddDynamic(this, &AJumpPadActor::OnJumpPadEndOverlap);

    // 초기 위치 저장 (애니메이션용)
    OriginalJumpPadLocation = JumpPadMesh->GetRelativeLocation();

    // 초기 머티리얼 설정
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

    // 점프 패드 비활성화
    SetJumpPadActive(false);

    // 목적지가 있으면 해당 점프패드도 비활성화
    if (DestinationJumpPad)
    {
        DestinationJumpPad->SetJumpPadActive(false);
    }

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

        // 이동 중이던 캐릭터가 있으면 마킹 해제
        if (CurrentTransportingCharacter)
        {
            
           // UnmarkActorJumped(CurrentTransportingCharacter);
            CurrentTransportingCharacter = nullptr;
        }
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
    if (JumpPadMesh)
    {
        UMaterialInterface* MatToApply = bIsJumpPadActive ? ActiveMaterial : InactiveMaterial;
        if (MatToApply)
        {
            JumpPadMesh->SetMaterial(0, MatToApply);
        }
    }
}

bool AJumpPadActor::IsDestinationClear(ACharacter* Character)
{
    if (!DestinationJumpPad || !Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Destination JumpPad or Character"));
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

    // 모든 클라이언트에서 애니메이션 시작

    if (!HasAuthority())
    {
        // 클라이언트에서는 시각 효과만 처리
        return;
    }
    // 최근 점프했다면 리턴
    //IsRecentlyJumped(Character)
    if (CurrentTransportingCharacter)
    {
        return;
    }

    // 점프 요청 실행 (서버 함수 호출)
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
        // 필요에 따라 타이머 정리 및 액터 활성화
        if (GetWorldTimerManager().IsTimerActive(CooldownTimerHandle))
        {
            GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
        }

        // 점프 패드 활성화 - 사용자가 점프 패드를 벗어나고 전송 중이 아니면
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
        
        // 점프 성공 시 점프 상태 마킹
       // MarkActorAsJumped(Character);

        // 성공 시 효과 트리거
        Multicast_OnJumpEffect(Character);
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
        UE_LOG(LogTemp, Warning, TEXT("Jump failed: Destination is blocked"));
        return false;
    }

    // 캐릭터 Z축 방향으로 밀어내기
    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp)
    {
        Multicast_PlayJumpPadAnimation();
        // Z축으로 점프력 적용
        MovementComp->AddImpulse(FVector(0, 0, JumpForce), true);
        //Character->LaunchCharacter(FVector(0, 0, JumpForce), false, true);
    }

    if (DestinationJumpPad)
    {
        //StartCharacterTransport(Character);
        FTimerHandle TimerHandle;

        // 람다 함수를 사용하여 0.5초 후에 StartCharacterTransport(Character)를 호출하도록 설정합니다.
        GetWorldTimerManager().SetTimer(TimerHandle, [this, Character]()
            {
                StartCharacterTransport(Character);
            }, 0.5f, false);
    }

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
        // 목적지에 도달하면 이동 완료
        // 크래시 방지를 위해 여기서 목적지 위치로 설정하지 않고 정지 시킴
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
        MovementComp->SetMovementMode(MOVE_Walking);
        MovementComp->DisableMovement();
    }

    // 타이머 시작
    StartTransportTimer(Character);
}

void AJumpPadActor::CompleteTransport()
{
    if (!CurrentTransportingCharacter)
    {
        StopTransportTimer();
        return;
    }

    // 타이머 종료
    GetWorldTimerManager().ClearTimer(TransportTimerHandle);

    // 캐릭터의 방향을 목적지 방향으로 설정
    if (DestinationJumpPad)
    {
        FTransform DestinationTransform = DestinationJumpPad->DestinationLocation->GetComponentTransform();

        // 마지막 위치를 조절 (크래시 방지)
        FVector SafeLocation = DestinationJumpPad->DestinationLocation->GetComponentLocation();
        SafeLocation += DestinationJumpPad->DestinationLocation->GetForwardVector() * DestinationOffset;

        // 안전하게 텔레포트 (충돌 무시)
        CurrentTransportingCharacter->SetActorLocation(SafeLocation, false, nullptr, ETeleportType::TeleportPhysics);

        // 방향 설정
        OrientCharacterAfterTransport(CurrentTransportingCharacter, DestinationTransform);
    }

    // 캐릭터 무브먼트 다시 활성화
    UCharacterMovementComponent* MovementComp = CurrentTransportingCharacter->GetCharacterMovement();
    if (MovementComp)
    {
        MovementComp->SetDefaultMovementMode();
        MovementComp->SetMovementMode(MOVE_Falling);
    }

    // 이동 완료 후 클린업
    ACharacter* CompletedCharacter = CurrentTransportingCharacter;
    CurrentTransportingCharacter = nullptr;

    // 점프 패드 활성화
    SetJumpPadActive(true);

    // 목적지 점프패드도 활성화
    if (DestinationJumpPad)
    {
        DestinationJumpPad->SetJumpPadActive(true);
    }

    // 점프된 액터 마킹 해제 (재사용 가능)
   // UnmarkActorJumped(CompletedCharacter);
}
void AJumpPadActor::TransportCharacter(float DeltaTime)
{
    if (!CurrentTransportingCharacter || !DestinationJumpPad)
    {
        return;
    }

    // 목적지로 천천히 보간
    FVector CurrentLocation = CurrentTransportingCharacter->GetActorLocation();

    // 목적지 계산 (오프셋 적용)
    FVector TargetLocation = DestinationJumpPad->DestinationLocation->GetComponentLocation();
    FVector ForwardDir = DestinationJumpPad->DestinationLocation->GetForwardVector();
    TargetLocation += ForwardDir * DestinationOffset;

    // 방향 벡터 계산
    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

    // 이번 프레임에 이동할 거리 계산
    float MoveDistance = TransportSpeed * DeltaTime;
    FVector NewLocation = CurrentLocation + Direction * MoveDistance;

    // 목적지에 도달했는지 확인
    float DistanceToTarget = FVector::Distance(CurrentLocation, TargetLocation);
    if (DistanceToTarget <= MoveDistance * 1.5f)
    {
        // 목적지에 도달하면 이동 완료
        NewLocation = TargetLocation;
        CompleteTransport();
    }

    // 캐릭터 위치 업데이트
    CurrentTransportingCharacter->SetActorLocation(NewLocation);
}
void AJumpPadActor::OrientCharacterAfterTransport(ACharacter* Character, const FTransform& DestinationTransform)
{
    if (!Character || !HasAuthority())
    {
        return;
    }

    // 캐릭터의 회전을 목적지 방향으로 설정
    FRotator NewRotation = DestinationTransform.GetRotation().Rotator();

    // 컨트롤러 회전 설정
    if (Character->GetController())
    {
        Character->GetController()->SetControlRotation(NewRotation);
    }

    // 캐릭터 속도 방향 조정
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
    // 이미 애니메이션 중이면 무시
    if (bIsAnimating)
        return;
    bIsAnimating = true;
    OriginalJumpPadLocation = JumpPadMesh->GetRelativeLocation(); // 현재 위치를 기준으로 설정

    // 애니메이션 시작 시간 저장
    AnimationStartTime = GetWorld()->GetTimeSeconds();

    // 타이머 설정
    GetWorldTimerManager().SetTimer(
        AnimationUpdateTimerHandle,
        this,
        &AJumpPadActor::UpdateJumpPadAnimation,
        0.016f,
        true);
}
void AJumpPadActor::UpdateJumpPadAnimation()
{
    if (!bIsAnimating)
        return;


    // 실제 경과 시간 계산 (타이머 elapsed 시간 대신)
    float ElapsedTime = GetWorld()->GetTimeSeconds() - AnimationStartTime;
    float Alpha = FMath::Clamp(ElapsedTime / AnimationDuration, 0.0f, 1.0f);

    // 사인 커브를 사용하여 부드러운 애니메이션 생성
    float HeightOffset = JumpPadAnimHeight * FMath::Sin(Alpha * PI);

    // 메시 위치 업데이트
    JumpPadMesh->SetRelativeLocation(OriginalJumpPadLocation + FVector(0, 0, HeightOffset));

    // 애니메이션 완료 체크
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
        // 애니메이션이 시작될 때
        OriginalJumpPadLocation = JumpPadMesh->GetRelativeLocation();
        AnimationStartTime = GetWorld()->GetTimeSeconds();

        // 애니메이션 업데이트 타이머 시작
        GetWorldTimerManager().SetTimer(
            AnimationUpdateTimerHandle,
            this,
            &AJumpPadActor::UpdateJumpPadAnimation,
            0.016f,
            true);
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
    //GetWorldTimerManager().ClearTimer(AnimationUpdateTimerHandle);
    //GetWorldTimerManager().ClearTimer(AnimationCompleteTimerHandle);

    //// 메시 원래 위치로 복원
    //JumpPadMesh->SetRelativeLocation(OriginalJumpPadLocation);

    //// 애니메이션 상태 초기화
    //bIsAnimating = false;
    GetWorldTimerManager().ClearTimer(AnimationUpdateTimerHandle);

    // 메시 원래 위치로 복원
    JumpPadMesh->SetRelativeLocation(OriginalJumpPadLocation);

    // 애니메이션 상태 초기화
    bIsAnimating = false;
}

void AJumpPadActor::Multicast_OnJumpEffect_Implementation(ACharacter* Character)
{
    // 모든 클라이언트에서 실행될 점프 이펙트 코드
    FVector EffectLocation = GetActorLocation();

    // Particle 재생
    if (JumpEffectParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), JumpEffectParticle, EffectLocation);
    }

    // 사운드 재생
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

    // 디버그 시각화
#if WITH_EDITOR
    if (Character)
    {
        DrawDebugSphere(GetWorld(), Character->GetActorLocation(), 100.0f, 16, FColor::Blue, false, 1.0f);
    }
#endif
}

void AJumpPadActor::OnCooldownComplete()
{
    if (HasAuthority() && !CurrentTransportingCharacter)
    {
        // 쿨다운 완료 후 점프패드 다시 활성화
        SetJumpPadActive(true);
    }
}