// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestCharacter.h" // Changed include to TestCharacter.h
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Components/ItemComponent/ItemComponent.h"
#include "Components/SkillComponent/SkillComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "WeaponMaster/Items/InteractionComponent/InteractionComponent.h"

DEFINE_LOG_CATEGORY(LogTestCharacter); // Changed Log Category Name

ATestCharacter::ATestCharacter() // Renamed constructor
{
    PrimaryActorTick.bCanEverTick = true;

    // 컴포넌트 생성
    ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
    SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
    // 기본값 설정
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    bLimitYMovement = true;
    YMovementLimit = 10.0f;
    InteractableActor = nullptr;

    // 네트워크 복제 설정
    bReplicates = true;
    GetCharacterMovement()->SetIsReplicated(true);
}

void ATestCharacter::BeginPlay() // Renamed class method
{
    Super::BeginPlay();

    if (ItemComponent && SkillComponent)
    {
        // 아이템 장착 이벤트 바인딩
        ItemComponent->OnItemEquipped.AddDynamic(this, &ATestCharacter::OnItemEquipped);
        ItemComponent->OnItemUnequipped.AddDynamic(this, &ATestCharacter::OnItemUnequipped);
    }
    
    // Enhanced Input 활성화 (BeginPlay에서 하는 것이 일반적)
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0); // DefaultMappingContext UMappingContext 에셋 설정 필요
        }
    }
}

void ATestCharacter::Tick(float DeltaTime) // Renamed class method
{
    Super::Tick(DeltaTime);

    // 횡스크롤 이동 제한 적용
    if (bLimitYMovement)
    {
        EnforceMovementLimits();
    }
}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) // Renamed class method
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced Input Component 캐스팅
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Move 액션 매핑
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestCharacter::EnhancedMove); // MoveAction UInputAction 에셋 설정 필요

        // 액션 입력 바인딩 (기존 액션 입력은 그대로 유지)
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATestCharacter::Jump); // JumpAction 도 필요하면 Enhanced Input Action으로 변경
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATestCharacter::OnInteractInput);

        // 스킬 단축키 액션 매핑
        if (Skill1Action)
        {
            EnhancedInputComponent->BindAction(Skill1Action, ETriggerEvent::Started, this, &ATestCharacter::UseSkill1);
        }

        if (Skill2Action)
        {
            EnhancedInputComponent->BindAction(Skill2Action, ETriggerEvent::Started, this, &ATestCharacter::UseSkill2);
        }

        if (Skill3Action)
        {
            EnhancedInputComponent->BindAction(Skill3Action, ETriggerEvent::Started, this, &ATestCharacter::UseSkill3);
        }

        if (Skill4Action)
        {
            EnhancedInputComponent->BindAction(Skill4Action, ETriggerEvent::Started, this, &ATestCharacter::UseSkill4);
        }
    }
}

float ATestCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) // Renamed class method
{
    float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f)
    {
        // 체력 감소
        CurrentHealth = FMath::Max(0.0f, CurrentHealth - ActualDamage);

        // 상태이상 적용 (DamageType에 따라)
        if (DamageEvent.DamageTypeClass)
        {
            // DamageType에서 상태이상 정보 추출
            UDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
            // 추출된 정보로 상태이상 적용 (실제 구현에서는 커스텀 DamageType 클래스 필요)
        }

        // 사망 처리
        if (CurrentHealth <= 0.0f)
        {
            // 사망 처리 로직
        }
    }

    return ActualDamage;
}

bool ATestCharacter::EquipItem(FName ItemID) // Renamed class method
{
    if (ItemComponent)
    {
        return ItemComponent->EquipItem(ItemID);
    }
    return false;
}

void ATestCharacter::OnItemEquipped(UItemDataAsset* EquippedItem)
{
    if (SkillComponent && EquippedItem)
    {
        // 스킬 초기화
        SkillComponent->InitializeSkillsFromItem(EquippedItem);
    }
}

void ATestCharacter::OnItemUnequipped()
{
    if (SkillComponent)
    {
        // 스킬 초기화 (null 전달하여 스킬 제거)
        SkillComponent->InitializeSkillsFromItem(nullptr);
    }
}

/**
 * 인덱스 기반으로 스킬을 실행합니다.
 *
 * @param SkillIndex 실행할 스킬의 인덱스
 */
void ATestCharacter::ExecuteSkill(int32 SkillIndex) // Renamed class method
{
    if (SkillComponent)
    {
        SkillComponent->ExecuteSkill(SkillIndex);
    }
}

// Enhanced Input Move 함수
void ATestCharacter::EnhancedMove(const FInputActionValue& Value) // Renamed class method
{

    // 입력 값 추출 (float -1.0 ~ 1.0)
    float MoveValue = Value.Get<float>();

    if (MoveValue != 0.0f)
    {
        // 오른쪽/왼쪽으로 이동
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), MoveValue);
    }

}

void ATestCharacter::Jump() // Renamed class method
{

    Super::Jump();

}

void ATestCharacter::UseSkill1() // Renamed class method
{

    ExecuteSkill(0); // 인덱스 0번 스킬 실행

}

/**
 * 스킬 2 단축키 함수 (인덱스 1번 스킬 실행)
 */
void ATestCharacter::UseSkill2() // Renamed class method
{

    ExecuteSkill(1); // 인덱스 1번 스킬 실행

}

/**
 * 스킬 3 단축키 함수 (인덱스 2번 스킬 실행)
 */
void ATestCharacter::UseSkill3() // Renamed class method
{

    ExecuteSkill(2); // 인덱스 2번 스킬 실행

}

/**
 * 스킬 4 단축키 함수 (인덱스 3번 스킬 실행)
 */
void ATestCharacter::UseSkill4() // Renamed class method
{

    ExecuteSkill(3); // 인덱스 3번 스킬 실행

}

void ATestCharacter::Server_ApplyDamage_Implementation(AActor* Target, float Damage, TSubclassOf<UDamageType> DamageType) // Renamed class method
{
    if (Target && DamageType)
    {
        FDamageEvent DamageEvent(DamageType);
        Target->TakeDamage(Damage, DamageEvent, GetController(), this);
    }
}

void ATestCharacter::OnInteractInput() // Renamed class method
{
    // 상호작용 가능한 아이템이 있는지 확인
    if (InteractableActor)
    {
        // 상호작용 컴포넌트 찾기
        UInteractionComponent* InteractionComp = InteractableActor->FindComponentByClass<UInteractionComponent>();
        if (InteractionComp)
        {
            // 상호작용 실행
            InteractionComp->Interact(this);
        }
    }
}

void ATestCharacter::SetInteractableActor(AActor* NewInteractableActor) // Renamed class method
{
    InteractableActor = NewInteractableActor;
}

void ATestCharacter::InterruptActiveSkill()
{
    // SkillComponent가 없으면 리턴
    if (!SkillComponent)
    {
        return;
    }

    // 현재 활성화된 스킬 배열 가져오기
    TArray<UBaseSkill*> CurrentSkills = SkillComponent->GetSkills();
    
    // 활성화된 스킬이 있는지 확인하고 중단
    for (UBaseSkill* Skill : CurrentSkills)
    {
        if (Skill && Skill->IsSkillActive())
        {
            // 애니메이션 몽타주 중단
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
                if (CurrentMontage)
                {
                    AnimInstance->Montage_Stop(0.25f, CurrentMontage);
                }
            }
            
            // 스킬 강제 종료
            Skill->EndSkill();
            
            // 디버그 로그
            UE_LOG(LogTestCharacter, Display, TEXT("Skill %s was interrupted due to damage"), *Skill->GetSkillName());
            break;
        }
    }
}

void ATestCharacter::EnforceMovementLimits() // Renamed class method
{
    FVector CurrentLocation = GetActorLocation();

    // Y축 이동 제한
    if (FMath::Abs(CurrentLocation.Y) > YMovementLimit)
    {
        CurrentLocation.Y = FMath::Clamp(CurrentLocation.Y, -YMovementLimit, YMovementLimit);
        SetActorLocation(CurrentLocation);
    }
}



void ATestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const // Renamed class method
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 체력 복제
    DOREPLIFETIME(ATestCharacter, CurrentHealth); // Renamed class in DOREPLIFETIME
}