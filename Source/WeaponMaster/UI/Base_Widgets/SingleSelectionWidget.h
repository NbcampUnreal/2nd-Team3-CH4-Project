#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "GameFramework/Character.h"
#include "SingleSelectionWidget.generated.h"

// 전방 선언
class UNameBoxWidget;
class UCharacterBoxWidget;

/**
 * 단일 선택 위젯 - 여러 개의 아이템 중 하나를 선택할 수 있는 위젯
 */
UCLASS()
class WEAPONMASTER_API USingleSelectionWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    USingleSelectionWidget(const FObjectInitializer& ObjectInitializer);
    
    virtual void NativeConstruct() override;
    
    // ItemBox 위젯들을 설정하는 함수
    UFUNCTION(BlueprintCallable, Category = "Setup")
    void SetupItemBoxes(const TArray<FName>& ItemNames, const TArray<TSubclassOf<ACharacter>>& CharacterClasses);
    
    // 다음 메뉴로 이동 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextMenu);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnNextMenu OnNextMenu;
    
    // 이전 메뉴로 이동 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrevMenu);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPrevMenu OnPrevMenu;
    
    // 아이템 선택 시 호출될 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSelected, FName, SelectedItemName);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnItemSelected OnItemSelected;
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterClassSelected, TSubclassOf<ACharacter>, SelectedCharacterClass);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCharacterClassSelected OnCharacterClassSelected;
    
protected:
    // 위젯 컴포넌트들
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* CanvasPanel;
    
    UPROPERTY(meta = (BindWidget))
    UButton* NextButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* PrevButton;
    
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* UniformGridPanel;
    
    // 위젯 클래스들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    TSubclassOf<UNameBoxWidget> NameBoxClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    TSubclassOf<UCharacterBoxWidget> CharacterBoxClass;

    // 데이터 저장용 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<FName> ItemNames;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<TSubclassOf<ACharacter>> CharacterClasses;
    
    // 버튼 이벤트 핸들러
    UFUNCTION()
    void OnNextButtonClicked();
    
    UFUNCTION()
    void OnPrevButtonClicked();
    
    // 아이템 클릭 이벤트 핸들러
    UFUNCTION()
    void OnItemNameClicked(FName ItemName);
    
    UFUNCTION()
    void OnCharacterClassClicked(TSubclassOf<ACharacter> CharacterClass);
    
    // 그리드 슬롯 패딩
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FMargin SlotPadding;
    
    // 그리드 열 수 (한 행에 몇 개의 아이템을 표시할지)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    int32 GridColumns = 10;
};