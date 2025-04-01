// Copyright (c) 2024 Fergius Engineering.

#include "GLSCustomCategoriesWidgetBase.h"

#include <Kismet/GameplayStatics.h>
#include <TimerManager.h>

#include "GLSCustomCategoryModel.h"
#include "GLSListView.h"
#include "GLSSaveSystemConstants.h"
#include "GLSSaveSystemUtils.h"
#include "GLSSettings.h"
#include "GLSWidgetBase.h"

void UGLSCustomCategoriesWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    if (!IsValid(this) || !this->IsValidLowLevel() || !this->IsValidLowLevelFast())
    {
        return;
    }

    ensure(CategoryTpe != EGLSCustomCategoriesTypes::None);

    if (auto Settings = UGLSSettings::Get())
    {
        if (Settings->bEnableLayoutSave && Settings->bEnableCustomCategorySave)
        {
            bEnableSaveForCategories = Settings->CustomCategoriesToLoad & (uint8)CategoryTpe;
        }
    }

    check(ListView);
    ListView->OnItemClicked().AddUObject(this, &UGLSCustomCategoriesWidgetBase::OnCategoryItemClicked);
    ListView->OnItemDoubleClicked().AddUObject(this, &UGLSCustomCategoriesWidgetBase::OnCategoryItemDoubleClicked);

    SubscribeToMainLogWidget();

    const FLogsFullData& LogsData = UGLSUtils::GetLogsFullData(this);
    for (const auto& LogPtr : LogsData.RecievedLogs)
    {
        OnLogRecieved(LogPtr);
    }

    if (auto Subsystem = UGLSSubsystem::Get())
    {
        Subsystem->OnLogRecieved.AddUObject(this, &UGLSCustomCategoriesWidgetBase::OnLogRecieved);
    }
}

TMap<FString, FString> UGLSCustomCategoriesWidgetBase::PrepareDataToSave_Implementation()
{
    TMap<FString, FString> Params;

    for (const auto Mode : ActiveEditableMode)
    {
        const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, FString::FromInt((uint8)Mode.Key), GLS::Constants::GLS_SAVE_PARAM_ENABLE_MODE);
        if (!ParamName.IsEmpty())
        {
            Params.Add(ParamName, FString::FromInt(Mode.Value));
        }
    }

    for (const auto Model : CetGategoryModels())
    {
        if (IsValid(Model))
        {
            // enable category
            {
                const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, Model->GetCategoryName().ToString(), GLS::Constants::GLS_SAVE_PARAM_CUSTOM_CATEGORY_ENABLE);
                Params.Add(ParamName, FString::FromInt(bEnableSaveForCategories && Model->GetCategoryEnable()));
            }

            // enable editable mode favorite
            {
                const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, Model->GetCategoryName().ToString(), GLS::Constants::GLS_SAVE_PARAM_ENABLE_FAVORITE);
                Params.Add(ParamName, FString::FromInt(Model->GetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Favorite)));
            }

            // enable editable mode visibility
            {
                const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, Model->GetCategoryName().ToString(), GLS::Constants::GLS_SAVE_PARAM_ENABLE_VISIBILITY);
                Params.Add(ParamName, FString::FromInt(Model->GetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Hidden)));
            }
        }
    }

    // CetegoriesSortType
    {
        const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(UGLSCustomCategoriesWidgetBase, CetegoriesSortType), GLS::Constants::GLS_SAVE_PARAM_CATEGORY_SORT_TYPE);
        Params.Add(ParamName, FString::FromInt((int32)CetegoriesSortType));
    }

    // bToolsPanelVisible
    {
        const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(UGLSCustomCategoriesWidgetBase, bToolsPanelVisible), GLS::Constants::GLS_SAVE_PARAM_ENABLE_VISIBILITY);
        Params.Add(ParamName, bToolsPanelVisible ? TEXT("true") : TEXT("false"));
    }

    return Params;
}

void UGLSCustomCategoriesWidgetBase::SubscribeToMainLogWidget()
{
    if (IsValid(MainLogsWidget) && MainLogsWidget->IsValidLowLevel() && MainLogsWidget->IsValidLowLevelFast())
    {
        MainLogsWidget->OnFiltersCleared.AddDynamic(this, &UGLSCustomCategoriesWidgetBase::OnOnFiltersCleared);
    }
    else
    {
        MainLogsWidget = nullptr;

        if (auto World = GetWorld())
        {
            World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
                {
                    SubscribeToMainLogWidget();
                }));
        }
    }
}

void UGLSCustomCategoriesWidgetBase::InitCategoriesFromSave()
{
    if (bEnableSaveForCategories)
    {
        const FString ClassName = GetClass()->GetName();
        auto SaveDataForCategories = UGLSSaveSystemUtils::GetSaveValuesBySaveTag(this, {ClassName, GLS::Constants::GLS_SAVE_PARAM_CUSTOM_CATEGORY_ENABLE});

        for (const auto& Pair : SaveDataForCategories)
        {
            TArray<FString> ParsedArray;

            Pair.Key.ParseIntoArray(ParsedArray, TEXT("~"), true);

            if (ParsedArray.IsValidIndex(1))
            {
                const FString SubString = ParsedArray[1];
                if (!SubString.IsEmpty())
                {
                    AddCategoryItem(*SubString);
                }
            }
        }
    }
}

void UGLSCustomCategoriesWidgetBase::OnLogRecieved(const TSharedPtr<FGLSLogInfo>& LogPtr)
{
    FDateTime LastClearOnPIE;
    if (auto Settings = UGLSSettings::Get())
    {
        if (!UGLSUtils::IsEditorContext(this) && Settings->bClearLogsOnPIE)
        {
            LastClearOnPIE = UGLSSubsystem::Get()->GetLastBeginPIETime();
        }
        if (UGLSUtils::IsEditorContext(this) && Settings->bClearEditorLogsOnPIE)
        {
            LastClearOnPIE = UGLSSubsystem::Get()->GetLastBeginPIETime();
        }
    }

    if (LogPtr->GetTime() >= LastClearOnPIE)
    {
        FName Category = NAME_None;

        switch (CategoryTpe)
        {
        case EGLSCustomCategoriesTypes::None:
            ensure(false);
            break;
        case EGLSCustomCategoriesTypes::Categories:
            Category = LogPtr->Category;
            break;
        case EGLSCustomCategoriesTypes::Classes:
            Category = LogPtr->GetClassName();
            break;
        case EGLSCustomCategoriesTypes::Objects:
            Category = LogPtr->GetObjectName();
            break;
        case EGLSCustomCategoriesTypes::Tags:
            for (const auto& Tag : LogPtr->Tags)
            {
                AddCategoryItem(Tag);
            }
            return;
        default:
            ensure(false);
            break;
        }

        AddCategoryItem(Category);
    }
}

void UGLSCustomCategoriesWidgetBase::RequestApplySave_Implementation()
{
    // get from save EditableModeType::Favorite
    {
        const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, FString::FromInt((uint8)EGLSCustomCetegoryEditableModeType::Favorite), GLS::Constants::GLS_SAVE_PARAM_ENABLE_MODE);
        bool bParamValue = false;
        UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue);
        ActiveEditableMode.Add({EGLSCustomCetegoryEditableModeType::Favorite, bParamValue});
    }

    // get from save EditableModeType::Visibility
    {
        const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, FString::FromInt((uint8)EGLSCustomCetegoryEditableModeType::Hidden), GLS::Constants::GLS_SAVE_PARAM_ENABLE_MODE);
        bool bParamValue = false;
        UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue);
        ActiveEditableMode.Add({EGLSCustomCetegoryEditableModeType::Hidden, bParamValue});
    }

    // CetegoriesSortType
    {
        const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(UGLSCustomCategoriesWidgetBase, CetegoriesSortType), GLS::Constants::GLS_SAVE_PARAM_CATEGORY_SORT_TYPE);
        int32 bParamValue = false;
        UGLSSaveSystemUtils::GetSaveValueAsInteger(this, ParamName, bParamValue);
        CetegoriesSortType = (EGLSCustomCetegorySortType)bParamValue;
    }

    // bToolsPanelVisible
    {
        const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(UGLSCustomCategoriesWidgetBase, bToolsPanelVisible), GLS::Constants::GLS_SAVE_PARAM_ENABLE_VISIBILITY);
        bool bParamValue = false;
        if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
        {
            bToolsPanelVisible = bParamValue;
        }
    }

    if (auto World = GetWorld())
    {
        World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
            {
                InitCategoriesFromSave();
            }));
    }
}

void UGLSCustomCategoriesWidgetBase::AddCategoryItem(const FName& NewCategoryName)
{
    if (!ContainsCategory(NewCategoryName) && NewCategoryName != NAME_None)
    {
        UGLSCustomCategoryModel* Model = NewObject<UGLSCustomCategoryModel>(this);
        Model->SetCategoryName(NewCategoryName);

        Model->Number = CategoryModels.Add(Model);

        Model->OnVariableChanged.AddUObject(this, &UGLSCustomCategoriesWidgetBase::OnModelVariableChanged);
        Model->OnEnabledEditableMode.AddUObject(this, &UGLSCustomCategoriesWidgetBase::OnEnabledEditableModeChanged);

        Model->SetActiveEditableModes(ActiveEditableMode);

        //Enable enabled category from save
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, Model->GetCategoryName().ToString(), GLS::Constants::GLS_SAVE_PARAM_CUSTOM_CATEGORY_ENABLE);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                Model->SetCategoryEnable(bEnableSaveForCategories && bParamValue);
            }
        }

        //enable EnableEditableMode favorite from save
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, Model->GetCategoryName().ToString(), GLS::Constants::GLS_SAVE_PARAM_ENABLE_FAVORITE);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                Model->SetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Favorite, bParamValue);
            }
        }

        //enable EnableEditableMode visibility from save
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, Model->GetCategoryName().ToString(), GLS::Constants::GLS_SAVE_PARAM_ENABLE_VISIBILITY);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                Model->SetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Hidden, bParamValue);
            }
        }

        SortModelsAndRecreateView();
    }
}

bool UGLSCustomCategoriesWidgetBase::ContainsCategory(const FName& Category)
{
    return CategoryModels.ContainsByPredicate([&Category](UGLSCustomCategoryModel* Model)
        {
            return IsValid(Model) && Model->GetCategoryName() == Category;
        });
}

void UGLSCustomCategoriesWidgetBase::EnableCategoryTextOnLogs(const bool bEnable)
{
    unimplemented();
}

void UGLSCustomCategoriesWidgetBase::SetActiveEditableMode(const EGLSCustomCetegoryEditableModeType ModeType, const bool bEnabled)
{
    if (ensure(ActiveEditableMode.Contains(ModeType)))
    {
        ActiveEditableMode[ModeType] = bEnabled;
    }

    for (auto* Model : CetGategoryModels())
    {
        if (IsValid(Model))
        {
            Model->SetActiveEditableMode(ModeType, bEnabled);
        }
    }
}

bool UGLSCustomCategoriesWidgetBase::GetActiveEditableMode(const EGLSCustomCetegoryEditableModeType ModeType) const
{
    if (ActiveEditableMode.Contains(ModeType))
    {
        return ActiveEditableMode[ModeType];
    }

    return false;
}

void UGLSCustomCategoriesWidgetBase::SetCetegoriesSortType(const EGLSCustomCetegorySortType SortType)
{
    CetegoriesSortType = SortType;

    SortModelsAndRecreateView();
}

void UGLSCustomCategoriesWidgetBase::SelectOrDeselectAllCategories()
{
    int32 EnabledNum = 0;
    int32 VisibilityDisabledNum = 0;
    for (auto* Model : ListView->GetListItems())
    {
        const auto CategoryModel = Cast<UGLSCustomCategoryModel>(Model);
        if (IsValid(CategoryModel))
        {
            if (CategoryModel->GetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Hidden))
            {
                VisibilityDisabledNum++;
                continue;
            }

            if (CategoryModel->GetCategoryEnable())
            {
                EnabledNum++;
            }
        }
    }

    for (auto* Model : ListView->GetListItems())
    {
        const auto CategoryModel = Cast<UGLSCustomCategoryModel>(Model);
        if (IsValid(CategoryModel))
        {
            bool bEnable = true;

            // all categories is enabled, need disable all
            if (EnabledNum == (ListView->GetListItems().Num() - VisibilityDisabledNum))
            {
                bEnable = false;
            }

            // NOTE: Skip items with visibility is false, as it means they do not need to have visibility enable.
            if (bEnable && CategoryModel->GetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Hidden))
            {
                continue;
            }

            CategoryModel->SetCategoryEnable(bEnable);
        }
    }
}

void UGLSCustomCategoriesWidgetBase::SetSearchText(const FText& InSearchText)
{
    SearchText = InSearchText;

    SortModelsAndRecreateView();
}

void UGLSCustomCategoriesWidgetBase::InitializeCategories(const TArray<FName>& Categories)
{
    for (const auto& Category : Categories)
    {
        AddCategoryItem(Category);
    }
}

int32 UGLSCustomCategoriesWidgetBase::GetNumEnabledCategories() const
{
    int32 Num = 0;
    for (const auto Model : CetGategoryModels())
    {
        if (IsValid(Model) && Model->GetCategoryEnable())
        {
            Num++;
        }
    }
    return Num;
}

void UGLSCustomCategoriesWidgetBase::ClearCategories()
{
    OnOnFiltersCleared();
    CategoryModels.Empty();
    SortModelsAndRecreateView();
}

void UGLSCustomCategoriesWidgetBase::DisableCategories()
{
    for (const auto Model : CetGategoryModels())
    {
        if (IsValid(Model))
        {
            Model->SetCategoryEnable(false);
        }
    }
}

void UGLSCustomCategoriesWidgetBase::OnModelVariableChanged(const UGLSCustomCategoryModel* Model, const FName& VariableName)
{
    if (GET_MEMBER_NAME_CHECKED(UGLSCustomCategoryModel, CategoryEnable) == VariableName)
    {
        ApplyFiltersByModel(Model);
    }
}

void UGLSCustomCategoriesWidgetBase::OnEnabledEditableModeChanged(const UGLSCustomCategoryModel* Model, const EGLSCustomCetegoryEditableModeType Mode, const bool bEnabled)
{
    SortModelsAndRecreateView();
}

void UGLSCustomCategoriesWidgetBase::OnOnFiltersCleared()
{
    for (auto Model : CetGategoryModels())
    {
        Model->SetCategoryEnable(false);
    }
}

void UGLSCustomCategoriesWidgetBase::SortModelsAndRecreateView()
{
    // 1 - Find all items where favorite is true and visibility is true.
    // 2 - Find all items where favorite is false and visibility is true.
    // 3 - Find all items where favorite is true and visibility is false.
    // 4 - Find all items where favorite is false and visibility is false.
    // 5 - Sort each group by SortType.
    // 6 - Combine the sorted groups into a final array.
    // 7 - Recreate the list view with the final array and apply filters to logs.

    // Lambda to filter models based on favorite and visibility flags
    auto FilterModels = [this](const TArray<TObjectPtr<class UGLSCustomCategoryModel>>& Models, bool Favorite, bool Hidden)
    {
        return Models.FilterByPredicate([Favorite, Hidden, this](const TObjectPtr<class UGLSCustomCategoryModel>& Model)
            {
                // clang-format off
                return IsValid(Model) 
                    && Model->GetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Favorite) == Favorite 
                    && Model->GetEnableEditableMode(EGLSCustomCetegoryEditableModeType::Hidden) == Hidden 
                    ;
                // clang-format on
            });
    };

    // Lambda to sort models based on the SortType
    auto SortModels = [this](TArray<TObjectPtr<class UGLSCustomCategoryModel>>& Models)
    {
        switch (CetegoriesSortType)
        {
        case EGLSCustomCetegorySortType::Newest:
            Models.Sort([](const UGLSCustomCategoryModel& A, const UGLSCustomCategoryModel& B)
                {
                    return A.Number > B.Number;
                });
            break;
        case EGLSCustomCetegorySortType::Oldest:
            Models.Sort([](const UGLSCustomCategoryModel& A, const UGLSCustomCategoryModel& B)
                {
                    return A.Number < B.Number;
                });
            break;
        case EGLSCustomCetegorySortType::AZ:
            Models.Sort([](const UGLSCustomCategoryModel& A, const UGLSCustomCategoryModel& B)
                {
                    return A.GetCategoryName().ToString() < B.GetCategoryName().ToString();
                });
            break;
        case EGLSCustomCetegorySortType::ZA:
            Models.Sort([](const UGLSCustomCategoryModel& A, const UGLSCustomCategoryModel& B)
                {
                    return A.GetCategoryName().ToString() > B.GetCategoryName().ToString();
                });
            break;
        default:
            unimplemented();
            break;
        }
    };

    // Lambda to add models to the ListView
    auto AddItemsToList = [this](const TArray<TObjectPtr<class UGLSCustomCategoryModel>>& Models)
    {
        for (const auto& Model : Models)
        {
            ListView->AddItem(Model);
            ApplyFiltersByModel(Model);
        }
    };

    const auto FilteredModelsBySearchText = CategoryModels.FilterByPredicate([this](const TObjectPtr<class UGLSCustomCategoryModel>& Model)
        {
            return SearchText.IsEmpty() ? true : Model->GetCategoryName().ToString().Contains(SearchText.ToString());
        });

    // Filter models into groups
    TArray<TObjectPtr<class UGLSCustomCategoryModel>> Filter1 = FilterModels(FilteredModelsBySearchText, true, false);
    TArray<TObjectPtr<class UGLSCustomCategoryModel>> Filter2 = FilterModels(FilteredModelsBySearchText, false, false);
    TArray<TObjectPtr<class UGLSCustomCategoryModel>> Filter3 = FilterModels(FilteredModelsBySearchText, true, true);
    TArray<TObjectPtr<class UGLSCustomCategoryModel>> Filter4 = FilterModels(FilteredModelsBySearchText, false, true);

    // Sort each group
    SortModels(Filter1);
    SortModels(Filter2);
    SortModels(Filter3);
    SortModels(Filter4);

    // Clear existing items and add sorted items to the ListView
    ListView->ClearListItems();
    AddItemsToList(Filter1);
    AddItemsToList(Filter2);
    AddItemsToList(Filter3);
    AddItemsToList(Filter4);
}

void UGLSCustomCategoriesWidgetBase::OnCategoryItemClicked(UObject* Model)
{
    if (auto CategoryModel = Cast<UGLSCustomCategoryModel>(Model))
    {
        CategoryModel->ChangeCategoryEnable();
    }
}

void UGLSCustomCategoriesWidgetBase::OnCategoryItemDoubleClicked(UObject* Model)
{
    if (auto CategoryModel = Cast<UGLSCustomCategoryModel>(Model))
    {
        CategoryModel->ChangeCategoryEnable();
    }

    SelectOrDeselectAllCategories();
}