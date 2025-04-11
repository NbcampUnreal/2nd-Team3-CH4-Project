#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "ItemBoxBase.generated.h"

/**
 * 아이템 박스 위젯의 기본 클래스 - 공통 기능을 제공합니다
 */
UCLASS(Abstract)
class WEAPONMASTER_API UItemBoxBase : public UUserWidget
{
	GENERATED_BODY()
    
public:
	UItemBoxBase(const FObjectInitializer& ObjectInitializer);
    
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
    
	// 위젯 크기 설정 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemBoxSize(float Width, float Height);
	
	// 보더에 이미지 설정 함수 추가
	UFUNCTION(BlueprintCallable, Category = "Item|Appearance")
	void SetBorderImage(UTexture2D* Image);
	
	// 보더 배경색 설정
	UFUNCTION(BlueprintCallable, Category = "Item|Appearance")
	void SetBorderColor(const FLinearColor& Color);
    
    // 선택 상태 설정 함수 추가
    UFUNCTION(BlueprintCallable, Category = "Item|Selection")
    void SetSelected(bool bInSelected);
    
    // 선택 상태 확인 함수 추가
    UFUNCTION(BlueprintPure, Category = "Item|Selection")
    bool IsSelected() const { return bSelected; }
    
protected:
	// 마우스 이벤트 핸들러
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    
	// 위젯 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox;
    
	UPROPERTY(meta = (BindWidget))
	UBorder* Border;
	
	// 테두리 보더 추가
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Outline;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock;
    
	// 기본 위젯 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float BoxWidth = 200.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float BoxHeight = 200.0f;
    
	// 호버 시 Border 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FLinearColor HoverColor = FLinearColor(0.8f, 0.8f, 1.0f, 1.0f);
    
	// 일반 상태의 Border 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FLinearColor NormalColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // 선택 상태 추적
    UPROPERTY(BlueprintReadOnly, Category = "Selection")
    bool bSelected = false;
};