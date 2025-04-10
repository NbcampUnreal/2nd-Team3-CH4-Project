#include "SingleSelectionWidget.h"
#include "NameBoxWidget.h"
#include "CharacterBoxWidget.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Kismet/GameplayStatics.h" // GameplayStatics 추가

USingleSelectionWidget::USingleSelectionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 기본값 초기화
    SlotPadding = FMargin(5.0f); // 슬롯 패딩 (기본값)
    GridColumns = 10; // 기본 열 수
    CurrentScreenType = ESelectionScreenType::Character; // 기본 화면은 캐릭터 선택
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
    
    // 초기 화면 설정 (제목 업데이트 포함)
    if (CurrentScreenType == ESelectionScreenType::Character)
    {
        SwitchToCharacterSelection();
    }
    else
    {
        SwitchToItemSelection();
    }
}

UWeaponMasterGameInstance* USingleSelectionWidget::GetWeaponMasterGameInstance() const
{
    return Cast<UWeaponMasterGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
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
                int32 Row = (_ItemNames.Num() + i) / GridColumns;
                int32 Column = (_ItemNames.Num() + i) % GridColumns;
                UniformGridPanel->AddChildToUniformGrid(CharBox, Row, Column);
            }
        }
    }
}

void USingleSelectionWidget::OnNextButtonClicked()
{
    if (CurrentScreenType == ESelectionScreenType::Character)
    {
        // 캐릭터가 선택되었는지 확인
        UWeaponMasterGameInstance* GameInstance = GetWeaponMasterGameInstance();
        if (GameInstance && GameInstance->CharacterClass != nullptr)
        {
            // 아이템 선택 화면으로 전환
            SwitchToItemSelection();
        }
        else
        {
            // 캐릭터를 선택하라는 메시지 표시
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("캐릭터를 먼저 선택해주세요"));
            }
        }
    }
    else if (CurrentScreenType == ESelectionScreenType::Item)
    {
        // 아이템이 선택되었는지 확인
        UWeaponMasterGameInstance* GameInstance = GetWeaponMasterGameInstance();
        if (GameInstance && !GameInstance->ItemName.IsNone())
        {
            // 게임 레벨로 이동
            UGameplayStatics::OpenLevel(GetWorld(), GameLevelName);
        }
        else
        {
            // 아이템을 선택하라는 메시지 표시
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("아이템을 먼저 선택해주세요"));
            }
        }
    }
    
    // 원래 델리게이트 호출 (필요에 따라 사용)
    OnNextMenu.Broadcast();
}

void USingleSelectionWidget::OnPrevButtonClicked()
{
    if (CurrentScreenType == ESelectionScreenType::Character)
    {
        // 이전 레벨(메인 메뉴 등)로 이동
        UGameplayStatics::OpenLevel(GetWorld(), PreviousLevelName);
    }
    else if (CurrentScreenType == ESelectionScreenType::Item)
    {
        // 캐릭터 선택 화면으로 돌아가기
        SwitchToCharacterSelection();
    }
    
    // 원래 델리게이트 호출 (필요에 따라 사용)
    OnPrevMenu.Broadcast();
}

void USingleSelectionWidget::SwitchToCharacterSelection()
{
    // 화면 상태 변경
    CurrentScreenType = ESelectionScreenType::Character;
    
    // 그리드 패널 초기화
    if (UniformGridPanel)
    {
        UniformGridPanel->ClearChildren();
    }
    
    // 캐릭터 선택 UI 설정
    TArray<FName> EmptyNames;
    SetupItemBoxes(EmptyNames, CharacterClasses);
    
    // 화면 제목 업데이트
    UpdateScreenTitle();
}

void USingleSelectionWidget::SwitchToItemSelection()
{
    // 화면 상태 변경
    CurrentScreenType = ESelectionScreenType::Item;
    
    // 그리드 패널 초기화
    if (UniformGridPanel)
    {
        UniformGridPanel->ClearChildren();
    }
    
    // 아이템 선택 UI 설정
    TArray<TSubclassOf<ACharacter>> EmptyClasses;
    SetupItemBoxes(ItemNames, EmptyClasses);
    
    // 화면 제목 업데이트
    UpdateScreenTitle();
}

void USingleSelectionWidget::UpdateScreenTitle()
{
    // 제목 텍스트 블록이 있으면 업데이트
    if (TitleTextBlock)
    {
        if (CurrentScreenType == ESelectionScreenType::Character)
        {
            TitleTextBlock->SetText(CharacterSelectionTitle);
        }
        else
        {
            TitleTextBlock->SetText(ItemSelectionTitle);
        }
    }
}

void USingleSelectionWidget::OnItemNameClicked(FName ItemName)
{
    // 아이템 이름을 로그에 출력
    UE_LOG(LogTemp, Log, TEXT("선택된 아이템: %s"), *ItemName.ToString());
    
    // 게임 인스턴스에 선택된 아이템 이름 저장
    UWeaponMasterGameInstance* GameInstance = GetWeaponMasterGameInstance();
    if (GameInstance)
    {
        GameInstance->ItemName = ItemName;
        UE_LOG(LogTemp, Log, TEXT("게임 인스턴스에 아이템 이름 저장: %s"), *ItemName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스를 찾을 수 없습니다."));
    }
    
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
        
        // 게임 인스턴스에 선택된 캐릭터 클래스 저장
        UWeaponMasterGameInstance* GameInstance = GetWeaponMasterGameInstance();
        if (GameInstance)
        {
            GameInstance->CharacterClass = CharacterClass;
            UE_LOG(LogTemp, Log, TEXT("게임 인스턴스에 캐릭터 클래스 저장: %s"), *ClassName);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스를 찾을 수 없습니다."));
        }
    }
    
    // 캐릭터 클래스 선택 이벤트 브로드캐스트
    OnCharacterClassSelected.Broadcast(CharacterClass);
}