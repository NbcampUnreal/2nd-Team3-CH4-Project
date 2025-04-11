#pragma once

#include "CoreMinimal.h"
#include "ItemBoxBase.h"
#include "GameFramework/Character.h"
#include "CharacterBoxWidget.generated.h"

// 클릭 시 캐릭터 클래스를 반환하는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterBoxClicked, TSubclassOf<ACharacter>, CharacterClass, UCharacterBoxWidget*, Widget);

/**
 * 캐릭터 클래스 데이터를 위한, 캐릭터 클래스 이름을 표시하는 위젯
 */
UCLASS()
class WEAPONMASTER_API UCharacterBoxWidget : public UItemBoxBase
{
	GENERATED_BODY()
    
public:
	UCharacterBoxWidget(const FObjectInitializer& ObjectInitializer);
    
	// 캐릭터 클래스 설정 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetCharacterClass(TSubclassOf<ACharacter> InCharacterClass);
    
	// 클래스 이름 업데이트
	UFUNCTION(BlueprintCallable, Category = "Item")
	void UpdateDisplayName();
    
	// 클릭 이벤트
	UFUNCTION()
	void OnItemClicked();

	// 캐릭터 클래스 반환 함수
	UFUNCTION(BlueprintPure, Category = "Item|Data")
	TSubclassOf<ACharacter> GetCharacterClass() const { return CharacterClass; }
	
	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Item|Event")
	FOnCharacterBoxClicked OnCharacterClassClicked;
    
protected:
	// 마우스 클릭 오버라이드
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    
	// 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<ACharacter> CharacterClass;
};