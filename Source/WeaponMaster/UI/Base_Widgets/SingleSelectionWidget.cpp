#include "SingleSelectionWidget.h"
#include "NameBoxWidget.h"
#include "CharacterBoxWidget.h"

USingleSelectionWidget::USingleSelectionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 기본값 초기화
    SlotPadding = FMargin(5.0f); // 슬롯 패딩 (기본값)
    GridColumns = 10; // 기본 열 수
}

void USingleSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 버튼 이벤트 바인딩
    if (NextButton)
    {
        NextButton->OnClicked.AddDynamic(this, &USingleSelectionWidget::OnNextButtonClicked);
    }
    
    if (PrevButton)
    {
        PrevButton->OnClicked.AddDynamic(this, &USingleSelectionWidget::OnPrevButtonClicked);
    }
    
    // 그리드 슬롯 패딩 설정
    if (UniformGridPanel)
    {
        UniformGridPanel->SetSlotPadding(SlotPadding);
    }

    SetupItemBoxes(ItemNames, CharacterClasses);
}

void USingleSelectionWidget::SetupItemBoxes(const TArray<FName>& _ItemNames, const TArray<TSubclassOf<ACharacter>>& _CharacterClasses)
{
    // 기존 위젯 제거
    if (UniformGridPanel)
    {
        UniformGridPanel->ClearChildren();
    }
    
    // 이름 위젯 추가
    for (int32 i = 0; i < _ItemNames.Num(); ++i)
    {
        if (NameBoxClass)
        {
            UNameBoxWidget* NameBox = CreateWidget<UNameBoxWidget>(this, NameBoxClass);
            if (NameBox)
            {
                NameBox->OnObjectNameClicked.AddDynamic(this, &USingleSelectionWidget::OnItemNameClicked);
                NameBox->SetObjectName(_ItemNames[i]);
                
                int32 Row = i / GridColumns;
                int32 Column = i % GridColumns;
                UniformGridPanel->AddChildToUniformGrid(NameBox, Row, Column);
            }
        }
    }
    
    // 캐릭터 클래스 위젯 추가
    for (int32 i = 0; i < _CharacterClasses.Num(); ++i)
    {
        if (CharacterBoxClass)
        {
            UCharacterBoxWidget* CharBox = CreateWidget<UCharacterBoxWidget>(this, CharacterBoxClass);
            if (CharBox)
            {
                CharBox->OnCharacterClassClicked.AddDynamic(this, &USingleSelectionWidget::OnCharacterClassClicked);
                CharBox->SetCharacterClass(_CharacterClasses[i]);
                
                // 여기에서는 별도의 행에 캐릭터 클래스 위젯을 추가합니다
                // 필요한 경우 다른 위치 계산 로직으로 변경할 수 있습니다
                int32 Row = (_ItemNames.Num() + i) / GridColumns;
                int32 Column = (_ItemNames.Num() + i) % GridColumns;
                UniformGridPanel->AddChildToUniformGrid(CharBox, Row, Column);
            }
        }
    }
}

void USingleSelectionWidget::OnNextButtonClicked()
{
    // 다음 메뉴로 이동 델리게이트 호출
    OnNextMenu.Broadcast();
}

void USingleSelectionWidget::OnPrevButtonClicked()
{
    // 이전 메뉴로 이동 델리게이트 호출
    OnPrevMenu.Broadcast();
}

void USingleSelectionWidget::OnItemNameClicked(FName ItemName)
{
    // 아이템 이름을 로그에 출력
    UE_LOG(LogTemp, Log, TEXT("선택된 아이템: %s"), *ItemName.ToString());
    
    // 아이템 선택 이벤트 브로드캐스트
    OnItemSelected.Broadcast(ItemName);
}

void USingleSelectionWidget::OnCharacterClassClicked(TSubclassOf<ACharacter> CharacterClass)
{
    // 캐릭터 클래스 이름을 로그에 출력
    if (CharacterClass)
    {
        FString ClassName = CharacterClass->GetName();
        UE_LOG(LogTemp, Log, TEXT("선택된 캐릭터 클래스: %s"), *ClassName);
    }
    
    // 캐릭터 클래스 선택 이벤트 브로드캐스트
    OnCharacterClassSelected.Broadcast(CharacterClass);
}