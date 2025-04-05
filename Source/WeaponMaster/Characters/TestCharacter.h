#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TestCharacter.generated.h" // Changed include and class name in generated include

struct FInputActionValue;
enum class EInputActionType : uint8;
class UItemComponent;
class UStatusComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTestCharacter, Log, All); // Changed Log Category Name

UCLASS(config=Game)
class ATestCharacter : public ACharacter // Renamed class to ATestCharacter
{
	GENERATED_BODY()

public:
    ATestCharacter(); // Renamed constructor

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** 데미지 처리 함수 (언리얼 기본 함수 오버라이드) */
    virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    /** 아이템 장착 함수 */
    UFUNCTION(BlueprintCallable, Category = "Items")
    bool EquipItem(FName ItemID);

    /** 스킬 실행 함수 (인덱스 기반) */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void ExecuteSkill(int32 SkillIndex);

    /** 입력 처리: 이동 (Enhanced Input) */
    void EnhancedMove(const FInputActionValue& Value); // Enhanced Input 함수

    /** 입력 처리: 점프 */
    void Jump() override;

    /** 스킬 1 단축키 함수 (인덱스 0 스킬) */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void UseSkill1();

    /** 스킬 2 단축키 함수 (인덱스 1 스킬) */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void UseSkill2();

    /** 스킬 3 단축키 함수 (인덱스 2 스킬) */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void UseSkill3();

    /** 스킬 4 단축키 함수 (인덱스 3 스킬) */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void UseSkill4();

    /** 입력 처리: 상호작용 */
    void OnInteractInput();

    /** 상호작용 가능한 액터 설정 */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetInteractableActor(AActor* NewInteractableActor);

    /** 현재 상호작용 가능한 액터 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Interaction")
    AActor* GetInteractableActor() const { return InteractableActor; }

    /** 서버에 데미지 적용 RPC */
    UFUNCTION(Server, Reliable)
    void Server_ApplyDamage(AActor* Target, float Damage, TSubclassOf<UDamageType> DamageType);

protected:
    /** 아이템 컴포넌트 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UItemComponent* ItemComponent;

    /** 캐릭터 체력 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float MaxHealth;

    /** 현재 체력 */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    /** 횡스크롤 이동 제한 (Y축) */
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    bool bLimitYMovement;

    /** Y축 이동 제한 값 */
    UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (EditCondition = "bLimitYMovement"))
    float YMovementLimit;

    /** 상호작용 가능한 액터 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    AActor* InteractableActor;

    /** 캐릭터 상태 기계 설정 */
    void SetupCharacterStateMachine();

    /** 캐릭터 이동 제한 처리 */
    void EnforceMovementLimits();

private:
    /** Enhanced Input Action - Move */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* MoveAction;

    /** Enhanced Input Action - Jump */ // JumpAction 추가
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* JumpAction;

    /** Enhanced Input Action - Jump */ // JumpAction 추가
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* InteractAction;

    /** Enhanced Input Action - Skill 1 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* Skill1Action;

    /** Enhanced Input Action - Skill 2 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* Skill2Action;

    /** Enhanced Input Action - Skill 3 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* Skill3Action;

    /** Enhanced Input Action - Skill 4 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* Skill4Action;

    /** Enhanced Input Mapping Context */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;
};