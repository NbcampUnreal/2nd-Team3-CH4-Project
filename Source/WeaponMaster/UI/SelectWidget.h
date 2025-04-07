// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetStateData.h"
#include "SelectWidget.generated.h"


UCLASS()
class WEAPONMASTER_API USelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "SelectWidget")
	void SetWidgetState(EWidgetState NewState);
	void UpdateHighlight(bool bIsHighlighted);
protected:
	
	UPROPERTY(meta = (BindWidget))
	class UImage* CharacterImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* HighlightImage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectWidget")
	class UTexture2D* CharacterDefaultTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectWidget")
	class UTexture2D* HighlightTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectWidget")
	class UTexture2D* WeaponDefaultTexture;

	void InitializeWidget();

};
