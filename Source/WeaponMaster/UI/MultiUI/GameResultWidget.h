// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

class UVerticalBox;
class UResultPlayerEntryWidget;
class UTexture2D;
class UUserSelectButton;

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

	UPROPERTY(meta = (BindWidget))
	UUserSelectButton* MainButton;

	// 결과 항목 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
	TSubclassOf<UResultPlayerEntryWidget> ResultEntryClass;
	UFUNCTION()
	void OnMainButtonClicked();
};