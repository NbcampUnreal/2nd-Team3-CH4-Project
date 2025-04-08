// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UISelectData.generated.h"

/**
* 테스트용 SelectionData 이고,
* 만일 테스트에 성공한다면 , 
* 해당 아이디를 기반으로 DataTable에서 썸네일 맵 경로 , 이름등을 찾아볼 생각입니다
* 
*/
USTRUCT(BlueprintType)
struct FPlayerSelectionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    int32 CharacterID = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    int32 WeaponID = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    int32 MapID = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    bool bIsReady = false;
};
