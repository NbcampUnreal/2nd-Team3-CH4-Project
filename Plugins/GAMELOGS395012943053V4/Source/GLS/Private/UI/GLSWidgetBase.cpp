// Copyright (c) 2024 Fergius Engineering.

#include "GLSWidgetBase.h"

#include <Components/ListView.h>
#include <Components/ScrollBox.h>
#include <Components/VerticalBox.h>
#include <Components/VerticalBoxSlot.h>
#include <JsonObjectConverter.h>
#include <Misc/FileHelper.h>
#include <Misc/MessageDialog.h>
#include <Runtime/Launch/Resources/Version.h>
#include <Templates/SharedPointer.h>

#include "GLSListView.h"
#include "GLSLogInfoObject.h"
#include "GLSLogItemBase.h"
#include "GLSLogModelsPoolSubsystem.h"
#include "GLSSaveSystemConstants.h"
#include "GLSSaveSystemUtils.h"
#include "GLSSettings.h"
#include "GLSSubsystem.h"

#if PLATFORM_WINDOWS
#include <Windows/AllowWindowsPlatformTypes.h>
#include <commdlg.h>
#include <Windows/HideWindowsPlatformTypes.h>
#endif

void UGLSWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    auto World = GetWorld();
    if (!World || World->bIsTearingDown)
    {
        return;
    }

    ProcessCreateLogs();
    ProcessFilters();
}

FReply UGLSWidgetBase::NativeOnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent)
{
    const FKey Key = InAnalogInputEvent.GetKey();
    auto World = GetWorld();
    if (Key == EKeys::Gamepad_RightY && ListView_Logs && World && !World->bIsTearingDown)
    {
        const float StickValue = InAnalogInputEvent.GetAnalogValue();
        if (StickValue != 0.f)
        {
            const float ScrollSpeedMultiplier = 200.0f;
            const float ScrollAmount = StickValue * ScrollSpeedMultiplier * GetWorld()->GetDeltaSeconds();
            ListView_Logs->SetScrollOffset(ListView_Logs->GetScrollOffset() + ScrollAmount);

            const int32 TotalItems = ListView_Logs->GetNumItems();
            const auto& Widgets = ListView_Logs->GetDisplayedEntryWidgets();
            if (TotalItems > 0 && Widgets.Num() > 0)
            {
                const int32 IndexToSelect = (Widgets.Num() - 1) / 2;
                if (Widgets.IsValidIndex(IndexToSelect))
                {
                    ListView_Logs->ClearSelection();
                    const UObject* NewSelectedItem = *ListView_Logs->ItemFromEntryWidget(*Widgets[IndexToSelect]);
                    if (IsValid(NewSelectedItem))
                    {
                        ListView_Logs->SetSelectedItem(NewSelectedItem);
                    }
                }
            }
        }

        return FReply::Handled();
    }

    return Super::NativeOnAnalogValueChanged(MyGeometry, InAnalogInputEvent);
}

void UGLSWidgetBase::OnGeneratedEntryWidget(UObject* Model, UUserWidget* Widget)
{
    if (UGLSLogItemBase* const LogWidget = Cast<UGLSLogItemBase>(Widget))
    {
        LogWidget->SetLogSettings(LogSettingsPtr);
        LogWidget->SetHighlightText(HighlightTextPtr);
    }
}

UGLSWidgetBase::UGLSWidgetBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UGLSWidgetBase::ClearLogs()
{
    LastSavedLogNum = LastCreatedLogNum;

    LogDataModels.Empty(LastSavedLogNum);
    MarkNeedRecreateLogFilters();
}

void UGLSWidgetBase::SetFollowLogs(const bool bEnable)
{
    bFollowingLogs = bEnable;

    if (bFollowingLogs)
    {
        ScrollToBottom();
    }
}

void UGLSWidgetBase::SetReceivingLogs(const bool bEnable)
{
    bReceivingLogs = bEnable;
}

void UGLSWidgetBase::SetHighlightText(const FText& InSearchText)
{
    if (HighlightTextPtr)
    {
        *HighlightTextPtr = InSearchText;
    }
}

bool UGLSWidgetBase::IsAutoSaveLayoutEnabled() const
{
    return UGLSSaveSystemUtils::GetEnableSaveDataByTabName(this, GetParentTabName());
}

void UGLSWidgetBase::SetAutoSaveLayoutEnabled(const bool bEnable)
{
    UGLSSaveSystemUtils::EnableSaveDataByTabName(this, GetParentTabName(), bEnable, true);
}

FString UGLSWidgetBase::GetActiveFiltersStr() const
{
    FString ReturnStr;

    //prepare categories
    FString Categories;
    for (const auto& Filter : Filters)
    {
        if (!Filter.Category.IsNone())
            Categories.Append(Filter.Category.ToString() + " ");
    }
    if (!Categories.IsEmpty())
    {
        ReturnStr.Append(TEXT("\n\nCategories: ") + Categories);
    }

    //prepare Verbosities
    FString Verbosities;
    for (const auto& Filter : Filters)
    {
        if (Filter.Verbosity != EGLSLogType::NoLogging)
            Verbosities.Append(ToString(static_cast<ELogVerbosity::Type>(Filter.Verbosity)) + FString(" "));
    }
    if (!Verbosities.IsEmpty())
    {
        ReturnStr.Append(TEXT("\n\nVerbosities: ") + Verbosities);
    }

    //prepare Classes
    FString Classes;
    for (const auto& Filter : Filters)
    {
        if (!Filter.ClassName.IsNone())
            Classes.Append(Filter.ClassName.ToString() + " ");
    }
    if (!Classes.IsEmpty())
    {
        ReturnStr.Append(TEXT("\n\nClasses: ") + Classes);
    }

    //prepare Objects
    FString Objects;
    for (const auto& Filter : Filters)
    {
        if (!Filter.ObjectName.IsNone())
            Objects.Append(Filter.ObjectName.ToString() + " ");
    }
    if (!Objects.IsEmpty())
    {
        ReturnStr.Append(TEXT("\n\nObjects: ") + Objects);
    }

    //prepare Tags
    FString Tags;
    for (const auto& Filter : Filters)
    {
        if (!Filter.Tag.IsNone())
            Tags.Append(Filter.Tag.ToString() + " ");
    }
    if (!Tags.IsEmpty())
    {
        ReturnStr.Append(TEXT("\n\nTags: ") + Tags);
    }

    return ReturnStr;
}

int32 UGLSWidgetBase::GetLogsCount() const
{
    return IsValid(ListView_Logs) ? ListView_Logs->GetNumItems() : 0;
}

void UGLSWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    LogSettingsPtr = MakeShared<FGLSLogSettings>();
    HighlightTextPtr = MakeShared<FText>();

    // Get log settings from save data
    {
        //LogSettingsPtr bLogsWithTime
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithTime), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsWithTime = bParamValue;
            }
        }

        //LogSettingsPtr bLogsWithFrame
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithFrame), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsWithFrame = bParamValue;
            }
        }

        //LogSettingsPtr bLogsWithCategory
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithCategory), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsWithCategory = bParamValue;
            }
        }

        //LogSettingsPtr bLogsWithVerbosity
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithVerbosity), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsWithVerbosity = bParamValue;
            }
        }

        //LogSettingsPtr bLogsWithObjectName
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithObjectName), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsWithObjectName = bParamValue;
            }
        }

        //LogSettingsPtr bLogsWithClassName
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithClassName), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsWithClassName = bParamValue;
            }
        }

        //LogSettingsPtr bLogsWithTags
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithTags), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsWithTags = bParamValue;
            }
        }

        //LogSettingsPtr bLogsDuplicates
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsDuplicates), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            bool bParamValue = false;
            if (UGLSSaveSystemUtils::GetSaveValueAsBoolean(this, ParamName, bParamValue))
            {
                LogSettingsPtr->bLogsDuplicates = bParamValue;
            }
        }
    }

    if (IsValid(ListView_Logs))
    {
        ListView_Logs->OnListViewScrolled().AddWeakLambda(this, [this](float ItemOffset, float DistanceRemaining)
            {
                bFollowingLogs = ItemOffset > LastScrollOffset && ListView_Logs->IsItemVisible(ListView_Logs->GetListItems().Last());
                LastScrollOffset = ItemOffset;
            });

        ListView_Logs->OnItemScrolledIntoView().AddWeakLambda(this, [this](UObject* ListItem, UUserWidget& EntryWidget)
            {
                if (ListView_Logs->GetNumItems() > 0 && IsValid(ListItem))
                {
                    bFollowingLogs = ListView_Logs->IsItemVisible(ListItem);
                }
                else
                {
                    bFollowingLogs = true;
                }
            });

        ListView_Logs->OnGeneratedEntryWidget.AddDynamic(this, &UGLSWidgetBase::OnGeneratedEntryWidget);
    }

    if (auto LogsSubsystem = UGLSSubsystem::Get())
    {
        LogsSubsystem->OnLogRecieved.AddUObject(this, &UGLSWidgetBase::OnLogReceived);

        for (auto& LogPtr : UGLSUtils::GetLogs())
        {
            OnLogReceived(LogPtr);
        }

        MarkNeedRecreateLogFilters();
    }

    if (auto ModelsSubsystem = UGLSLogModelsPoolSubsystem::Get(this))
    {
        ModelsSubsystem->OnModelsCleared.AddDynamic(this, &UGLSWidgetBase::OnLogModelsCleared);
    }
}

TMap<FString, FString> UGLSWidgetBase::PrepareDataToSave_Implementation()
{
    TMap<FString, FString> Params;

    // Prepare log settings to save data
    if (LogSettingsPtr)
    {
        //LogSettingsPtr bLogsWithTime
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithTime), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsWithTime));
        }

        //LogSettingsPtr bLogsWithFrame
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithFrame), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsWithFrame));
        }

        //LogSettingsPtr bLogsWithCategory
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithCategory), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsWithCategory));
        }

        //LogSettingsPtr bLogsWithVerbosity
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithVerbosity), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsWithVerbosity));
        }

        //LogSettingsPtr bLogsWithObjectName
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithObjectName), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsWithObjectName));
        }

        //LogSettingsPtr bLogsWithClassName
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithClassName), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsWithClassName));
        }

        //LogSettingsPtr bLogsWithTags
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsWithTags), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsWithTags));
        }

        //LogSettingsPtr bLogsDuplicates
        {
            const FString ParamName = UGLSSaveSystemUtils::MakeSaveParamName(this, GET_MEMBER_NAME_STRING_CHECKED(FGLSLogSettings, bLogsDuplicates), GLS::Constants::GLS_SAVE_PARAM_LOG_SETTINGS);
            Params.Add(ParamName, FString::FromInt(LogSettingsPtr->bLogsDuplicates));
        }
    }

    return Params;
}

void UGLSWidgetBase::ProcessFilters()
{
    if (bNeedRecreateLogFilters && IsValid(ListView_Logs))
    {
        if (auto Settings = UGLSSettings::Get())
        {
            const TArray<TSharedPtr<FGLSLogInfo>>& AllLogs = UGLSUtils::GetLogs();
            TArray<UGLSLogInfoObject*> ModelsToDisplay;
            ModelsToDisplay.Reserve(AllLogs.Num());

            FContainerFilters AllFilters(Filters);
            if (!UGLSUtils::IsEditorContext(this) && Settings->bClearLogsOnPIE)
            {
                AllFilters.LastClearOnPIE = UGLSSubsystem::Get()->GetLastBeginPIETime();
            }
            if (UGLSUtils::IsEditorContext(this) && Settings->bClearEditorLogsOnPIE)
            {
                AllFilters.LastClearOnPIE = UGLSSubsystem::Get()->GetLastBeginPIETime();
            }

            for (auto& LogData : AllLogs)
            {
                if (LogData->LogNum > LastSavedLogNum && LogData->LogNum <= LastCreatedLogNum)
                {
                    FFilteredLog* LogFilter = LogDataModels.Find(LogData->LogNum);
                    if (LogFilter)
                    {
                        LogFilter->bEnabled = CanApplyFilterToLog(LogData.ToSharedRef(), AllFilters);
                        if (LogFilter->bEnabled)
                        {
                            ModelsToDisplay.Add(LogFilter->ListObjectPtr);
                        }
                    }
                }
            }

            ListView_Logs->SetListItems(ModelsToDisplay);
        }

        bNeedRecreateLogFilters = false;
    }
}

void UGLSWidgetBase::ProcessCreateLogs()
{
    auto World = GetWorld();
    if (!World || World->bIsTearingDown)
    {
        return;
    }

    if (bReceivingLogs && !PendingToCreate.IsEmpty())
    {
        if (auto Settings = UGLSSettings::Get())
        {
            FContainerFilters AllFilters(Filters);
            if (!UGLSUtils::IsEditorContext(this) && Settings->bClearLogsOnPIE)
            {
                AllFilters.LastClearOnPIE = UGLSSubsystem::Get()->GetLastBeginPIETime();
            }
            if (UGLSUtils::IsEditorContext(this) && Settings->bClearEditorLogsOnPIE)
            {
                AllFilters.LastClearOnPIE = UGLSSubsystem::Get()->GetLastBeginPIETime();
            }

            const int32 LimitPerFrame = Settings ? Settings->MaxLogsPerFrame : 1024;
            if (PendingToCreate.Num() > LimitPerFrame)
            {
                TArray<TSharedPtr<FGLSLogInfo>> ExtractedElements;
                ExtractedElements.Append(PendingToCreate.GetData(), LimitPerFrame);

#if (ENGINE_MAJOR_VERSION > 5) || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5)
                PendingToCreate.RemoveAt(0, LimitPerFrame, EAllowShrinking::No);
#else
                PendingToCreate.RemoveAt(0, LimitPerFrame, false);
#endif
                uint32 CreatedLogsNum = 0;
                for (auto& Log : ExtractedElements)
                {
                    bool bAddedToListOut = false;
                    if (auto LogPtr = AddFilteredItemToList(Log, AllFilters, bAddedToListOut))
                    {
                        if (bAddedToListOut)
                        {
                            CreatedLogsNum++;
                        }
                    }
                }

                /**
                  * If there is 0 here, it means that there are logs but they do not need to be displayed.
                  * No logs were added in this frame, you can try to create logs again in the same frame.
                  * This will slow down the game flow a little, but will prevent a long wait for logs to be created.
                */
                if (CreatedLogsNum == 0)
                {
                    ProcessCreateLogs();
                }
            }
            else
            {
                bool bAddedToListOut = false;
                for (auto& Log : PendingToCreate)
                {
                    AddFilteredItemToList(Log, AllFilters, bAddedToListOut);
                }

                PendingToCreate.Empty(PendingToCreate.Num());
            }
        }
    }
}

void UGLSWidgetBase::ScrollToBottom()
{
    if (ListView_Logs->GetNumItems() != 0)
    {
        ListView_Logs->ClearSelection();

        ListView_Logs->RequestScrollItemIntoView(ListView_Logs->GetListItems().Last());

        if (auto World = GetWorld())
        {
            World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
                {
                    const UObject* Item = ListView_Logs->GetListItems().Last();
                    ListView_Logs->SetSelectedItem(Item);
                }));
        }
    }
}

void UGLSWidgetBase::ApplyFilterByClassName(const FName& ClassName, const bool bEnabled)
{
    Filters.RemoveAll([&](const FGLSFilterLog& It)
        {
            return It.ClassName == ClassName;
        });

    if (bEnabled)
    {
        FGLSFilterLog Filter{};
        Filter.ClassName = ClassName;
        Filters.Add(Filter);
    }

    MarkNeedRecreateLogFilters();
}

void UGLSWidgetBase::ApplyFilterByClassNames(const TArray<FName>& ClassNames, const bool bEnabled)
{
    for (const FName& ClassName : ClassNames)
    {
        ApplyFilterByClassName(ClassName, bEnabled);
    }
}

void UGLSWidgetBase::ApplyFilterByTag(const FName& Tag, const bool bEnabled)
{
    Filters.RemoveAll([Tag](const FGLSFilterLog& It)
        {
            return It.Tag == Tag;
        });

    if (bEnabled)
    {
        FGLSFilterLog Filter;
        Filter.Tag = Tag;
        Filters.Add(Filter);
    }

    MarkNeedRecreateLogFilters();
}

void UGLSWidgetBase::ApplyFilterByTags(const TArray<FName>& Tags, const bool bEnabled)
{
    for (const FName& Tag : Tags)
    {
        ApplyFilterByTag(Tag, bEnabled);
    }
}

void UGLSWidgetBase::ApplyFilterByText(const FText& TextFilter)
{
    Filters.RemoveAll([](const FGLSFilterLog& It)
        {
            return !It.ExpressionEvaluator.GetFilterText().IsEmpty();
        });

    if (!TextFilter.IsEmpty())
    {
        FGLSFilterLog Filter;
        Filter.ExpressionEvaluator.SetFilterText(TextFilter);
        Filters.Add(Filter);
    }

    SetHighlightText(TextFilter);

    MarkNeedRecreateLogFilters();
}

void UGLSWidgetBase::ApplyFilterByObjectName(const FName& ObjectName, const bool bEnabled)
{
    Filters.RemoveAll([ObjectName](const FGLSFilterLog& It)
        {
            return It.ObjectName == ObjectName;
        });

    if (bEnabled)
    {
        FGLSFilterLog Filter;
        Filter.ObjectName = ObjectName;
        Filters.Add(Filter);
    }

    MarkNeedRecreateLogFilters();
}

void UGLSWidgetBase::ApplyFiltersByObjectNames(const TArray<FName>& ObjectNames, const bool bEnabled)
{
    for (const FName& ObjectName : ObjectNames)
    {
        ApplyFilterByObjectName(ObjectName, bEnabled);
    }
}

void UGLSWidgetBase::ClearFilters()
{
    Filters.Empty();
    MarkNeedRecreateLogFilters();
    OnFiltersCleared.Broadcast();
}

void UGLSWidgetBase::CopyFiltered()
{
    FString LogsToCopy;

    TSharedPtr<FGLSLogSettings> OverrideSettingsPtr = MakeShared<FGLSLogSettings>();
    OverrideSettingsPtr->bLogsDuplicates = true;
    OverrideSettingsPtr->bLogsWithCategory = true;
    OverrideSettingsPtr->bLogsWithClassName = false;
    OverrideSettingsPtr->bLogsWithFrame = true;
    OverrideSettingsPtr->bLogsWithObjectName = false;
    OverrideSettingsPtr->bLogsWithTags = false;
    OverrideSettingsPtr->bLogsWithTime = true;
    OverrideSettingsPtr->bLogsWithVerbosity = true;

    const TArray<TSharedPtr<FGLSLogInfo>>& AllLogs = UGLSUtils::GetLogs();
    for (const auto& LogData : AllLogs)
    {
        FFilteredLog* LogFilter = LogDataModels.Find(LogData->LogNum);
        if (LogFilter && LogFilter->bEnabled)
        {
            LogsToCopy.Append(LogFilter->ListObjectPtr->GetFormattedString(OverrideSettingsPtr.ToSharedRef()) + '\n');
        }
    }

    if (!LogsToCopy.IsEmpty())
    {
        LogsToCopy.RemoveAt(LogsToCopy.Len() - 1);
    }

    FGLSUtils::CopyLogsToClipboard(LogsToCopy);
}

void UGLSWidgetBase::CopyFull()
{
    FString LogsToCopy;

    TSharedPtr<FGLSLogSettings> OverrideSettingsPtr = MakeShared<FGLSLogSettings>();
    OverrideSettingsPtr->bLogsDuplicates = true;
    OverrideSettingsPtr->bLogsWithCategory = true;
    OverrideSettingsPtr->bLogsWithClassName = false;
    OverrideSettingsPtr->bLogsWithFrame = true;
    OverrideSettingsPtr->bLogsWithObjectName = false;
    OverrideSettingsPtr->bLogsWithTags = false;
    OverrideSettingsPtr->bLogsWithTime = true;
    OverrideSettingsPtr->bLogsWithVerbosity = true;

    const TArray<TSharedPtr<FGLSLogInfo>>& AllLogs = UGLSUtils::GetLogs();
    for (const auto& LogData : AllLogs)
    {
        FFilteredLog* LogFilter = LogDataModels.Find(LogData->LogNum);
        if (LogFilter)
        {
            LogsToCopy.Append(LogFilter->ListObjectPtr->GetFormattedString(OverrideSettingsPtr.ToSharedRef()) + '\n');
        }
    }

    if (!LogsToCopy.IsEmpty())
    {
        LogsToCopy.RemoveAt(LogsToCopy.Len() - 1);
    }

    FGLSUtils::CopyLogsToClipboard(LogsToCopy);
}

void UGLSWidgetBase::SaveFullLogs()
{
#if PLATFORM_WINDOWS
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    TCHAR szFile[MAX_PATH] = TEXT("");
    FString DefaultFilename = FString::Printf(TEXT("GLSLogOutput-%s.log"), *FDateTime::Now().ToString());
    TCHAR szDefaultFile[MAX_PATH];
    FCString::Strcpy(szDefaultFile, *DefaultFilename);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(TCHAR);
    ofn.lpstrFilter = TEXT("Log Files (*.log)\0*.log\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.lpstrTitle = TEXT("Save Log As...");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    lstrcpy(szFile, szDefaultFile);

    if (GetSaveFileName(&ofn))
    {
        FString SelectedFile = FString(szFile);

        if (FPaths::GetExtension(SelectedFile).IsEmpty())
        {
            SelectedFile += TEXT(".log");
        }

        TSharedPtr<FGLSLogSettings> OverrideSettingsPtr = MakeShared<FGLSLogSettings>();
        OverrideSettingsPtr->bLogsDuplicates = true;
        OverrideSettingsPtr->bLogsWithCategory = true;
        OverrideSettingsPtr->bLogsWithClassName = false;
        OverrideSettingsPtr->bLogsWithFrame = true;
        OverrideSettingsPtr->bLogsWithObjectName = false;
        OverrideSettingsPtr->bLogsWithTags = false;
        OverrideSettingsPtr->bLogsWithTime = true;
        OverrideSettingsPtr->bLogsWithVerbosity = true;

        FArchive* LogFile = IFileManager::Get().CreateFileWriter(*SelectedFile);
        if (LogFile != nullptr)
        {
            const TArray<TSharedPtr<FGLSLogInfo>>& AllLogs = UGLSUtils::GetLogs();
            for (const auto& LogData : AllLogs)
            {
                FFilteredLog* LogFilter = LogDataModels.Find(LogData->LogNum);
                if (LogFilter)
                {
                    const FString TextLog = LogFilter->ListObjectPtr->GetFormattedString(OverrideSettingsPtr.ToSharedRef()) + LINE_TERMINATOR;
                    LogFile->Serialize(TCHAR_TO_ANSI(*TextLog), TextLog.Len());
                }
            }

            LogFile->Close();
            delete LogFile;
        }
    }
#endif
}

void UGLSWidgetBase::SaveFilteredLogs()
{
    unimplemented();
}

void UGLSWidgetBase::SaveGLSLogs()
{
    unimplemented();
}

void UGLSWidgetBase::OpenLogs()
{
    unimplemented();
}

void UGLSWidgetBase::OnLogReceived(const TSharedPtr<FGLSLogInfo>& LogInfo)
{
    PendingToCreate.Add(LogInfo);
}

class UGLSLogItemBase* UGLSWidgetBase::GetLastLogItem() const
{
    if (IsValid(ListView_Logs))
    {
        const int32 LastIndex = ListView_Logs->GetNumItems() - 1;
        auto& Widgets = ListView_Logs->GetDisplayedEntryWidgets();
        if (Widgets.IsValidIndex(LastIndex))
        {
            return Cast<UGLSLogItemBase>(Widgets[LastIndex]);
        }
    }

    return nullptr;
}

void UGLSWidgetBase::ApplyFilterByLogType(const EGLSLogType Verbosity, const bool bEnabled)
{
    Filters.RemoveAll([Verbosity](const FGLSFilterLog& It)
        {
            return It.Verbosity == Verbosity;
        });

    if (bEnabled)
    {
        FGLSFilterLog Filter;
        Filter.Verbosity = Verbosity;
        Filters.Add(Filter);
    }

    MarkNeedRecreateLogFilters();
}

void UGLSWidgetBase::ApplyFilterByLogTypes(const TArray<EGLSLogType>& LogTypes, const bool bEnabled)
{
    for (const auto& Verbosity : LogTypes)
    {
        ApplyFilterByLogType(Verbosity, bEnabled);
    }
}

void UGLSWidgetBase::ApplyFilterByCategory(const FName& Category, const bool bEnabled)
{
    Filters.RemoveAll([Category](const FGLSFilterLog& It)
        {
            return It.Category == Category;
        });

    if (bEnabled)
    {
        FGLSFilterLog Filter;
        Filter.Category = Category;
        Filters.Add(Filter);
    }

    MarkNeedRecreateLogFilters();
}

void UGLSWidgetBase::ApplyFilterByCategories(const TArray<FName>& Categories, const bool bEnabled)
{
    for (const auto& Category : Categories)
    {
        ApplyFilterByCategory(Category, bEnabled);
    }
}

void UGLSWidgetBase::MarkNeedRecreateLogFilters()
{
    bNeedRecreateLogFilters = true;
}

void UGLSWidgetBase::OnLogModelsCleared()
{
    if (IsValid(ListView_Logs))
    {
        ListView_Logs->ClearListItems();
    }

    LastSavedLogNum = 0;
    LastCreatedLogNum = 0;
    MarkNeedRecreateLogFilters();
    LogDataModels.Empty(LogDataModels.Num());
}

bool UGLSWidgetBase::SetVisibilityToLogWidgetByIndex(const int32 Idx, const bool bVisible)
{
    if (ListView_Logs)
    {
        auto& Widgets = ListView_Logs->GetDisplayedEntryWidgets();
        if (Widgets.IsValidIndex(Idx))
        {
            return SetVisibilityToLogWidget(Widgets[Idx], bVisible);
        }
    }

    return false;
}

bool UGLSWidgetBase::SetVisibilityToLogWidget(UWidget* Widget, const bool bVisible)
{
    if (IsValid(Widget))
    {
        const ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
        if (Widget->GetVisibility() != NewVisibility)
        {
            Widget->SetVisibility(NewVisibility);
            return true;
        }
    }

    return false;
}

UGLSLogInfoObject* UGLSWidgetBase::AddFilteredItemToList(TSharedPtr<FGLSLogInfo>& LogDataToAdd, const FContainerFilters& AllFilters, bool& bAddedToListOut)
{
    bAddedToListOut = false;

    if (auto ModelsSubsystem = UGLSLogModelsPoolSubsystem::Get(this))
    {
        bool bPoolCleared = false;
        UGLSLogInfoObject* LogInfoObject = ModelsSubsystem->CreateModel(LogDataToAdd, bPoolCleared);
        LogDataModels.Add(LogDataToAdd->LogNum, {true, LogInfoObject});
        LastCreatedLogNum = LogDataToAdd->LogNum;

        if (IsValid(ListView_Logs) && ListView_Logs->GetIndexForItem(LogInfoObject) == INDEX_NONE)
        {
            if (CanApplyFilterToLog(LogDataToAdd.ToSharedRef(), AllFilters))
            {
                bAddedToListOut = true;
                ListView_Logs->AddLogItem(LogInfoObject);

                if (bFollowingLogs)
                {
                    ScrollToBottom();
                }
            }
        }

        return LogInfoObject;
    }

    return nullptr;
}

bool UGLSWidgetBase::CanApplyFilterToLog(const TSharedRef<FGLSLogInfo>& LogRef, const FContainerFilters& AllFilters)
{
    if (LogRef->GetTime() < AllFilters.LastClearOnPIE)
    {
        return false;
    }

    // Check filters
    if (!AllFilters.ClassNameFilters.IsEmpty())
    {
        bool bCheckClassName = AllFilters.ClassNameFilters.ContainsByPredicate([&LogRef](const FGLSFilterLog& It)
            {
                return LogRef->GetClassName() == It.ClassName;
            });

        if (!bCheckClassName)
        {
            return false;
        }
    }

    if (!AllFilters.ObjectNameFilters.IsEmpty())
    {
        bool bCheckObjectName = AllFilters.ObjectNameFilters.ContainsByPredicate([&LogRef](const FGLSFilterLog& It)
            {
                return LogRef->GetObjectName() == It.ObjectName;
            });

        if (!bCheckObjectName)
        {
            return false;
        }
    }

    if (!AllFilters.CategoryFilters.IsEmpty())
    {
        bool bCheckCategories = AllFilters.CategoryFilters.ContainsByPredicate([&LogRef](const FGLSFilterLog& It)
            {
                return LogRef->Category == It.Category;
            });

        if (!bCheckCategories)
        {
            return false;
        }
    }

    if (!AllFilters.TagFilters.IsEmpty())
    {
        bool bCheckByTags = AllFilters.TagFilters.ContainsByPredicate([&LogRef](const FGLSFilterLog& It)
            {
                return LogRef->Tags.ContainsByPredicate([&](const FName& Tag)
                    {
                        return It.Tag == Tag;
                    });
            });

        if (!bCheckByTags)
        {
            return false;
        }
    }

    if (!AllFilters.VerbosityFilters.IsEmpty())
    {
        bool bCheckLogTypes = AllFilters.VerbosityFilters.ContainsByPredicate([&LogRef](const FGLSFilterLog& It)
            {
                return LogRef->Verbosity == It.Verbosity;
            });

        if (!bCheckLogTypes)
        {
            return false;
        }
    }

    if (!AllFilters.TextFilters.IsEmpty())
    {
        //TODO: need refactor, TestTextFilter UGLSLogInfoObject to FGLSLogInfo
        if (LogDataModels.Contains(LogRef->LogNum))
        {
            const TObjectPtr<class UGLSLogInfoObject>& LogObjectPtr = LogDataModels[LogRef->LogNum].ListObjectPtr;
            if (IsValid(LogObjectPtr))
            {
                bool bCheckTextFilter = AllFilters.TextFilters.ContainsByPredicate([&](const FGLSFilterLog& It)
                    {
                        return It.ExpressionEvaluator.TestTextFilter(FGLSLogFilter_ExpressionContext(LogObjectPtr, LogSettingsPtr.ToSharedRef()));
                    });

                if (!bCheckTextFilter)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    // If all checks passed, log can be shown
    return true;
}