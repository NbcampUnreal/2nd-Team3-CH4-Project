// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

class UVerticalBox;
class UResultPlayerEntryWidget;
class UTexture2D;

USTRUCT(BlueprintType)
struct FPlayerResultData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite)
	FString  Nickname;

	UPROPERTY(BlueprintReadWrite)
	int32 Kills;

	UPROPERTY(BlueprintReadWrite)
	int32 Damage;

	UPROPERTY(BlueprintReadWrite)
	int32 Gold;
};

UCLASS()
class WEAPONMASTER_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Result")
	void PopulatePlayerEntries(const TArray<FPlayerResultData>& ResultList);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerEntryList;

	// 결과 항목 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
	TSubclassOf<UResultPlayerEntryWidget> ResultEntryClass;
};