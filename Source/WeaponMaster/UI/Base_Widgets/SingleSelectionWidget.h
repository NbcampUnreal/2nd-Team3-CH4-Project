#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "GameFramework/Character.h"
#include "SingleSelectionWidget.generated.h"

class UItemBoxBase;
class UWeaponMasterGameInstance;
// 전방 선언
class UNameBoxWidget;
class UCharacterBoxWidget;

// 현재 화면 상태를 나타내는 열거형
UENUM(BlueprintType)
enum class ESelectionScreenType : uint8
{
    Character UMETA(DisplayName = "캐릭터 선택"),
    Item UMETA(DisplayName = "아이템 선택")
};

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
    
    // 게임 인스턴스에 접근하는 헬퍼 함수
    UFUNCTION(BlueprintCallable, Category = "GameInstance")
    UWeaponMasterGameInstance* GetWeaponMasterGameInstance() const;
    
    // 다음 메뉴로 이동 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextMenu);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnNextMenu OnNextMenu;
    
    // 이전 메뉴로 이동 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrevMenu);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPrevMenu OnPrevMenu;
    
    // 다음 UI로 전환 시 호출될 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchToNextUI, UUserWidget*, NextWidget);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSwitchToNextUI OnSwitchToNextUI;
    
    // 이전 UI로 전환 시 호출될 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchToPrevUI, UUserWidget*, PrevWidget);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSwitchToPrevUI OnSwitchToPrevUI;
    
    // 아이템 선택 시 호출될 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSelected, FName, SelectedItemName);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnItemSelected OnItemSelected;
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterClassSelected, TSubclassOf<ACharacter>, SelectedCharacterClass);
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCharacterClassSelected OnCharacterClassSelected;
    
    // 현재 화면 상태
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
    ESelectionScreenType CurrentScreenType;
    
    // 다음 화면으로 이동할 레벨 이름 (레벨 이동 모드에서 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    FName GameLevelName;
    
    // 이전 화면으로 돌아갈 레벨 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    FName PreviousLevelName;
    
    // UI 모드 설정 (true: 위젯 전환 / false: 레벨 전환)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    bool bUseUITransition = false;
    
    // 다음으로 전환할 위젯 클래스 (UI 모드에서 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    TSubclassOf<UUserWidget> NextWidgetClass;
    
    // 이전으로 전환할 위젯 클래스 (UI 모드에서 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    TSubclassOf<UUserWidget> PrevWidgetClass;
    
    // 화면 전환 함수
    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void SwitchToCharacterSelection();
    
    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void SwitchToItemSelection();
    
    // 화면 전환시 제목 업데이트
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateScreenTitle();
    
    // 다음 UI 위젯으로 전환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Navigation")
    UUserWidget* SwitchToNextWidget();
    
    // 이전 UI 위젯으로 전환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Navigation")
    UUserWidget* SwitchToPrevWidget();
    
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
    
    // 화면 제목 텍스트 블록 (선택사항)
    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* TitleTextBlock;
    
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

    UPROPERTY()
    UItemBoxBase* SelectedItemBox;
    
    // 버튼 이벤트 핸들러
    UFUNCTION()
    void OnNextButtonClicked();
    
    UFUNCTION()
    void OnPrevButtonClicked();
    
    // 아이템 클릭 이벤트 핸들러
    UFUNCTION()
    void OnItemNameClicked(FName ItemName, UNameBoxWidget* Widget);
    
    UFUNCTION()
    void OnCharacterClassClicked(TSubclassOf<ACharacter> CharacterClass, UCharacterBoxWidget* Widget);
    
    // 그리드 슬롯 패딩
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FMargin SlotPadding;
    
    // 그리드 열 수 (한 행에 몇 개의 아이템을 표시할지)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    int32 GridColumns = 10;
    
    // 화면 제목 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FText CharacterSelectionTitle = FText::FromString(TEXT("캐릭터 선택"));
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FText ItemSelectionTitle = FText::FromString(TEXT("아이템 선택"));
};