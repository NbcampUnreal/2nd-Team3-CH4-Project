#pragma once

#include "CoreMinimal.h"
#include "ItemBoxBase.h"
#include "NameBoxWidget.generated.h"

// 클릭 시 FName과 위젯 자신을 반환하는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNameBoxClicked, FName, ObjectName, UNameBoxWidget*, Widget);

/**
 * FName 데이터를 위한, 아이템 이름을 표시하는 위젯
 */
UCLASS()
class WEAPONMASTER_API UNameBoxWidget : public UItemBoxBase
{
	GENERATED_BODY()
    
public:
	UNameBoxWidget(const FObjectInitializer& ObjectInitializer);
    
	// FName 설정 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetObjectName(const FName& InObjectName);
    
	// 이름 형식 업데이트
	UFUNCTION(BlueprintCallable, Category = "Item")
	void UpdateDisplayName();
    
	// 클릭 이벤트
	UFUNCTION()
	void OnItemClicked();

	// 객체 이름 반환 함수
	UFUNCTION(BlueprintPure, Category = "Item|Data")
	FName GetObjectName() const { return ObjectName; }
	
	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Item|Event")
	FOnNameBoxClicked OnObjectNameClicked;
    
protected:
	// 마우스 클릭 오버라이드
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    
	// 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName ObjectName;
};