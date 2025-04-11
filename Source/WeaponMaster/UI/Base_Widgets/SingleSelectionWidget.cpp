#include "SingleSelectionWidget.h"
#include "NameBoxWidget.h"
#include "CharacterBoxWidget.h"
#include "Data/GameDataManager.h"
#include "Data/ItemDataAsset.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Kismet/GameplayStatics.h" // GameplayStatics 추가

USingleSelectionWidget::USingleSelectionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 기본값 초기화
    SlotPadding = FMargin(5.0f); // 슬롯 패딩 (기본값)
    GridColumns = 10; // 기본 열 수
    CurrentScreenType = ESelectionScreenType::Character; // 기본 화면은 캐릭터 선택
    SelectedItemBox = nullptr; // 선택된 아이템 박스 초기화
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
    
    // 게임 인스턴스 가져오기 (데이터 매니저 접근용)
    UWeaponMasterGameInstance* GameInstance = GetWeaponMasterGameInstance();
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance를 찾을 수 없습니다."));
        return;
    }
    
    UGameDataManager* GameDataManager = GameInstance->GetSubsystem<UGameDataManager>();
    if (!GameDataManager)
    {
        UE_LOG(LogTemp, Error, TEXT("GameDataManager를 찾을 수 없습니다."));
        return;
    }
    
    // 이름 위젯 추가 (아이템)
    for (int32 i = 0; i < _ItemNames.Num(); ++i)
    {
        if (NameBoxClass)
        {
            UNameBoxWidget* NameBox = CreateWidget<UNameBoxWidget>(this, NameBoxClass);
            if (NameBox)
            {
                // 아이템 데이터 에셋에서 이미지 가져오기
                UItemDataAsset* ItemData = GameDataManager->GetItemData(_ItemNames[i]);
                
                // 아이템 이미지가 있으면 Border 브러시에 설정
                if (ItemData && !ItemData->Icon.IsNull())
                {
                    // 아이콘 로드
                    UTexture2D* IconTexture = ItemData->Icon.LoadSynchronous();
                    if (IconTexture)
                    {
                        // 보더에 이미지 설정
                        NameBox->SetBorderImage(IconTexture);
                        
                        UE_LOG(LogTemp, Log, TEXT("아이템 %s의 이미지가 적용되었습니다."), *_ItemNames[i].ToString());
                    }
                }
                
                // 이벤트 바인딩 및 아이템 이름 설정 (매개변수 추가)
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
                // 이벤트 바인딩 (매개변수 추가)
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
            if (bUseUITransition)
            {
                // UI 위젯 전환 모드
                SwitchToNextWidget();
            }
            else
            {
                // 기존 레벨 전환 모드
                UGameplayStatics::OpenLevel(GetWorld(), GameLevelName);
            }
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
        if (bUseUITransition && PrevWidgetClass)
        {
            // UI 위젯 전환 모드
            SwitchToPrevWidget();
        }
        else
        {
            // 기존 레벨 전환 모드
            UGameplayStatics::OpenLevel(GetWorld(), PreviousLevelName);
        }
    }
    else if (CurrentScreenType == ESelectionScreenType::Item)
    {
        // 아이템 선택 화면에서는 항상 캐릭터 선택 화면으로 돌아감
        SwitchToCharacterSelection();
    }
    
    // 원래 델리게이트 호출 (필요에 따라 사용)
    OnPrevMenu.Broadcast();
}

void USingleSelectionWidget::SwitchToCharacterSelection()
{
    // 화면 상태 변경
    CurrentScreenType = ESelectionScreenType::Character;
    
    // 선택된 아이템 박스 초기화
    SelectedItemBox = nullptr;
    
    // 그리드 패널 초기화
    if (UniformGridPanel)
    {
        UniformGridPanel->ClearChildren();
    }
    
    // 캐릭터 선택 UI 설정
    TArray<FName> EmptyNames;
    SetupItemBoxes(EmptyNames, CharacterClasses);
    
    // 저장된 캐릭터 클래스가 있으면 선택 상태 복원
    UWeaponMasterGameInstance* GameInstance = GetWeaponMasterGameInstance();
    if (GameInstance && GameInstance->CharacterClass)
    {
        // 위젯 목록을 순회하며 해당 캐릭터 클래스와 일치하는 위젯 찾기
        for (int32 i = 0; i < UniformGridPanel->GetChildrenCount(); ++i)
        {
            UCharacterBoxWidget* CharBox = Cast<UCharacterBoxWidget>(UniformGridPanel->GetChildAt(i));
            if (CharBox && CharBox->GetCharacterClass() == GameInstance->CharacterClass)
            {
                // 선택 상태 설정
                CharBox->SetSelected(true);
                SelectedItemBox = CharBox;
                break;
            }
        }
    }
    
    // 화면 제목 업데이트
    UpdateScreenTitle();
}

void USingleSelectionWidget::SwitchToItemSelection()
{
    // 화면 상태 변경
    CurrentScreenType = ESelectionScreenType::Item;
    
    // 선택된 아이템 박스 초기화
    SelectedItemBox = nullptr;
    
    // 그리드 패널 초기화
    if (UniformGridPanel)
    {
        UniformGridPanel->ClearChildren();
    }
    
    // 아이템 선택 UI 설정
    TArray<TSubclassOf<ACharacter>> EmptyClasses;
    SetupItemBoxes(ItemNames, EmptyClasses);
    
    // 저장된 아이템 이름이 있으면 선택 상태 복원
    UWeaponMasterGameInstance* GameInstance = GetWeaponMasterGameInstance();
    if (GameInstance && !GameInstance->ItemName.IsNone())
    {
        // 위젯 목록을 순회하며 해당 아이템 이름과 일치하는 위젯 찾기
        for (int32 i = 0; i < UniformGridPanel->GetChildrenCount(); ++i)
        {
            UNameBoxWidget* NameBox = Cast<UNameBoxWidget>(UniformGridPanel->GetChildAt(i));
            if (NameBox && NameBox->GetObjectName() == GameInstance->ItemName)
            {
                // 선택 상태 설정
                NameBox->SetSelected(true);
                SelectedItemBox = NameBox;
                break;
            }
        }
    }
    
    // 화면 제목 업데이트
    UpdateScreenTitle();
}

void USingleSelectionWidget::UpdateScreenTitle()
{
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

UUserWidget* USingleSelectionWidget::SwitchToNextWidget()
{
    if (!NextWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("다음 위젯 클래스가 설정되지 않았습니다."));
        return nullptr;
    }
    
    // 새 위젯 생성
    UUserWidget* NextWidget = CreateWidget<UUserWidget>(GetWorld(), NextWidgetClass);
    if (!NextWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("다음 위젯을 생성하는데 실패했습니다."));
        return nullptr;
    }
    
    // 현재 위젯 숨기기
    SetVisibility(ESlateVisibility::Collapsed);
    
    // 새 위젯 화면에 추가
    NextWidget->AddToViewport();
    
    // 입력 모드 설정 (UI만 입력 받음)
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(NextWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;
    }
    
    // 델리게이트 호출
    OnSwitchToNextUI.Broadcast(NextWidget);
    
    return NextWidget;
}

UUserWidget* USingleSelectionWidget::SwitchToPrevWidget()
{
    if (!PrevWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("이전 위젯 클래스가 설정되지 않았습니다."));
        return nullptr;
    }
    
    // 새 위젯 생성 (현재 위젯과 동일한 소유자 사용)
    UUserWidget* PrevWidget = CreateWidget<UUserWidget>(this->GetOwningPlayer(), PrevWidgetClass);
    if (!PrevWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("이전 위젯을 생성하는데 실패했습니다."));
        return nullptr;
    }
    
    // 이전 위젯 화면에 추가
    PrevWidget->AddToViewport();
    
    // 입력 모드 설정 (UI만 입력 받음)
    APlayerController* PlayerController = this->GetOwningPlayer();
    if (PlayerController)
    {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(PrevWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;
    }
    
    // 현재 위젯을 뷰포트에서 완전히 제거
    this->RemoveFromParent();
    
    // 델리게이트 호출
    OnSwitchToPrevUI.Broadcast(PrevWidget);
    
    UE_LOG(LogTemp, Log, TEXT("위젯 전환 완료 (이전): %s → %s"), 
        *this->GetClass()->GetName(), 
        *PrevWidget->GetClass()->GetName());
    
    return PrevWidget;
}

void USingleSelectionWidget::OnItemNameClicked(FName ItemName, UNameBoxWidget* Widget)
{
    // 아이템 이름을 로그에 출력
    UE_LOG(LogTemp, Log, TEXT("선택된 아이템: %s"), *ItemName.ToString());
    
    // 기존에 선택된 아이템 박스가 있으면 선택 해제
    if (SelectedItemBox)
    {
        SelectedItemBox->SetSelected(false);
    }
    
    // 새로 선택된 아이템 박스 설정
    if (Widget)
    {
        Widget->SetSelected(true);
        SelectedItemBox = Widget;
    }
    
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

void USingleSelectionWidget::OnCharacterClassClicked(TSubclassOf<ACharacter> CharacterClass, UCharacterBoxWidget* Widget)
{
    // 캐릭터 클래스 이름을 로그에 출력
    if (CharacterClass)
    {
        FString ClassName = CharacterClass->GetName();
        UE_LOG(LogTemp, Log, TEXT("선택된 캐릭터 클래스: %s"), *ClassName);
        
        // 기존에 선택된 아이템 박스가 있으면 선택 해제
        if (SelectedItemBox)
        {
            SelectedItemBox->SetSelected(false);
        }
        
        // 새로 선택된 캐릭터 박스 설정
        if (Widget)
        {
            Widget->SetSelected(true);
            SelectedItemBox = Widget;
        }
        
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