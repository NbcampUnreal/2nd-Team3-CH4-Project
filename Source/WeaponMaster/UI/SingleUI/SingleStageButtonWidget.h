// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SingleStageButtonWidget.generated.h"

// 구조체 선언
class UImage;
class UTextBlock;
class UButton;

USTRUCT(BlueprintType)
struct FStageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MapID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* MapThumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MapDescription;
};

/**
 * 스테이지 하나를 나타내는 버튼 위젯
 */
UCLASS()
class WEAPONMASTER_API USingleStageButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부에서 맵 정보를 설정할 수 있는 함수
	UFUNCTION(BlueprintCallable)
	void SetStageInfo(const FStageInfo& Info);

	// 클릭 시 외부에 알리는 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageSelected, const FStageInfo&, SelectedInfo);

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnStageSelected OnStageSelected;

	UFUNCTION(BlueprintCallable)
	void SimulateClick();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* MapImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapNameText;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

private:
	FStageInfo CurrentStageInfo;

	UFUNCTION()
	void HandleClicked();
};