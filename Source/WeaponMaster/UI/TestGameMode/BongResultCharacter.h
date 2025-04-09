// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BongResultCharacter.generated.h"

UCLASS()
class WEAPONMASTER_API ABongResultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    ABongResultCharacter();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayRandomAnimation(bool bIsWinner);

    UFUNCTION(Client, Reliable)
    void Client_PlayRandomAnimation(bool bIsWinner);


protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TArray<UAnimMontage*> WinAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TArray<UAnimMontage*> LoseAnimations;
};
