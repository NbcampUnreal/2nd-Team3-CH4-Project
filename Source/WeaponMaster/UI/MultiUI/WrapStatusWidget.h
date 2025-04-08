// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WrapStatusWidget.generated.h"

class UVerticalBox;
class UPlayerStatusWidget;

UCLASS()
class WEAPONMASTER_API UWrapStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializePlayerStatus();

	UFUNCTION(BlueprintCallable)
	void InitializeDummyPlayerStatus(int32 TotalPlayers, int32 MyTeamNum);

	UFUNCTION(BlueprintCallable, Category = "UI")
	UVerticalBox* GetLeftTeamContainer() const { return LeftTeamContainer; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UVerticalBox* GetRightTeamContainer() const { return RightTeamContainer; }
protected:
	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> PlayerContainer;*/

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> LeftTeamContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> RightTeamContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float WidgetSpacing = 10.0f;
};
