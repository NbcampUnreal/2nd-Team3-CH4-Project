#include "UI/TestGameMode/BongResultCharacter.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"

ABongResultCharacter::ABongResultCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);
}
void ABongResultCharacter::PlayRandomAnimation(bool bIsWinner)
{
    // 승리 여부에 따라 사용할 애니메이션 배열 선택
    TArray<UAnimMontage*>& Animations = bIsWinner ? WinAnimations : LoseAnimations;
    if (Animations.Num() > 0)
    {
        UE_LOG(LogTemp, Display, TEXT("Character :: Animation Num is bigger than 0"));
        int32 Index = FMath::RandRange(0, Animations.Num() - 1);
        UAnimMontage* SelectedAnimation = Animations[Index];
        if (SelectedAnimation && GetMesh())
        {
            UE_LOG(LogTemp, Display, TEXT("Character :: Get Mesh"));
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                UE_LOG(LogTemp, Display, TEXT("Character :: AnimInstance exist"));
                AnimInstance->Montage_Play(SelectedAnimation);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Character :: AnimInstance is null"));
            }
        }
    }
}
void ABongResultCharacter::Client_PlayRandomAnimation_Implementation(bool bIsWinner)
{
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        FTimerDelegate::CreateUObject(this, &ABongResultCharacter::PlayRandomAnimation, bIsWinner),
        0.2f, // 딜레이 시간 (필요에 따라 조정)
        false
    );
}