// Copyright (c) 2024 Fergius Engineering.

#include "GLSEditor.h"

#include <Blueprint/WidgetTree.h>
#include <Framework/Application/SlateApplication.h>
#include <Framework/MultiBox/MultiBoxBuilder.h>
#include <ISettingsModule.h>
#include <Interfaces/IPluginManager.h>
#include <LevelEditor.h>
#include <Modules/ModuleManager.h>
#include <Slate/SlateGameResources.h>
#include <Styling/CoreStyle.h>
#include <Styling/SlateStyleRegistry.h>
#include <WidgetBlueprint.h>
#include <Widgets/Docking/SDockTab.h>

#include "GLSCommands.h"
#include "GLSOverlayWidgetBase.h"
#include "GLSSettings.h"
#include "GLSWidgetBase.h"

static const FName GLSTabName("Game logs system");

const FVector2D Icon40x40(40.0f, 40.0f);

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

#define LOCTEXT_NAMESPACE "FGLSEditorModule"

void FGLSEditorModule::StartupModule()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule != nullptr)
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "Game logs",
            LOCTEXT("GLSSettingsName", "Game Logs System (GLS)"),
            LOCTEXT("GLSSettingsNameDescription", "Configure the Game logs system plugin"),
            GetMutableDefault<UGLSSettings>());
    }

    Style = MakeShareable(new FSlateStyleSet("GameLogsSystemStyle"));
    Style->SetContentRoot(IPluginManager::Get().FindPlugin("GameLogsSystem")->GetBaseDir() / TEXT("Resources"));

    Style->Set("GameLogsSystem.OpenPluginWindow", new IMAGE_BRUSH(TEXT("GameLogsButtonIcon_40x"), Icon40x40));

    FSlateStyleRegistry::RegisterSlateStyle(*Style);

    if (FSlateApplication::IsInitialized())
    {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }

    FGLSCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FGLSCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FGLSEditorModule::OpenTabClicked),
        FCanExecuteAction());

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());

    TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
    MenuExtender->AddMenuExtension("Log", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FGLSEditorModule::AddMenuExtension));

    TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
    ToolbarExtender->AddToolBarExtension("Content", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FGLSEditorModule::AddToolbarExtension));

    LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);

    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(GLSTabName, FOnSpawnTab::CreateRaw(this, &FGLSEditorModule::CreateGLSPluginTab)).SetDisplayName(LOCTEXT("FGLSTabTitle", "Game Logs System")).SetMenuType(ETabSpawnerMenuType::Hidden);

    FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditor.OnMapChanged().AddRaw(this, &FGLSEditorModule::OnChangedTabWorld);
}

void FGLSEditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
    Builder.AddToolBarButton(FGLSCommands::Get().OpenPluginWindow);
}

TSharedRef<class SDockTab> FGLSEditorModule::CreateGLSPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    TSharedRef<SWidget> TabWidget = CreateUtilityWidget();

    // clang-format off
    TSharedRef<SDockTab> Tab = SNew(SDockTab)
                                .TabRole(ETabRole::NomadTab)
                                .Label(NSLOCTEXT("Game logs", "OutputTabTitle", "Game Logs System"))
                                [
                                    TabWidget
                                ];
    // clang-format on

    Tab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FGLSEditorModule::OnTabClosed));

    CreatedTab = Tab;

    return Tab;
}

void FGLSEditorModule::OnChangedTabWorld(UWorld* World, EMapChangeType MapChangeType)
{
    if (MapChangeType == EMapChangeType::TearDownWorld)
    {
        // We need to Delete the UMG widget if we are tearing down the World it was built with.
        if (CreatedUMGWidget && World == CreatedUMGWidget->GetWorld())
        {
            if (CreatedTab.IsValid())
            {
                CreatedTab.Pin()->SetContent(SNullWidget::NullWidget);
            }

            if (IsValid(CreatedUMGWidget) && CreatedUMGWidget->IsValidLowLevel())
            {
                CreatedUMGWidget->Rename(nullptr, GetTransientPackage());
            }
            CreatedUMGWidget = nullptr;
        }
    }
    else if (MapChangeType != EMapChangeType::SaveMap)
    {
        // Recreate the widget if we are loading a map or opening a new map
        RegenerateCreatedTab();
    }
}

void FGLSEditorModule::RegenerateCreatedTab()
{
    if (TSharedPtr<SDockTab> CreatedTabPinned = CreatedTab.Pin())
    {
        TSharedRef<SWidget> TabWidget = CreateUtilityWidget();
        CreatedTabPinned->SetContent(TabWidget);
    }
}

TSharedRef<SWidget> FGLSEditorModule::CreateUtilityWidget()
{
    if (IsValid(CreatedUMGWidget) && CreatedUMGWidget->IsValidLowLevel() && CreatedUMGWidget->IsValidLowLevelFast())
    {
        CreatedUMGWidget->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty);
    }

    CreatedUMGWidget = nullptr;

    if (UWorld* World = GEditor->GetEditorWorldContext().World())
    {
        if (auto Settings = UGLSSettings::Get())
        {
            UClass* WidgetClass = (UClass*)UGLSSettings::Get()->GLSMainClass;

            CreatedUMGWidget = CreateWidget<UUserWidget>(World, WidgetClass);
            if (CreatedUMGWidget)
            {
                // Editor Utility is flagged as transient to prevent from dirty the World it's created in when a property added to the Utility Widget is changed
                CreatedUMGWidget->SetFlags(RF_Transient);

                // Also mark nested utility widgets as transient to prevent them from dirtying the world (since they'll be created via CreateWidget and not CreateUtilityWidget)
                TArray<UWidget*> AllWidgets;
                CreatedUMGWidget->WidgetTree->GetAllWidgets(AllWidgets);

                for (UWidget* Widget : AllWidgets)
                {
                    if (Widget->IsA(UWidget::StaticClass()))
                    {
                        Widget->SetFlags(RF_Transient);
                        if (UPanelSlot* Slot = Widget->Slot)
                        {
                            Slot->SetFlags(RF_Transient);
                        }
                    }
                }

                return SNew(SVerticalBox) + SVerticalBox::Slot()
                                                .HAlign(HAlign_Fill)
                                                    [CreatedUMGWidget->TakeWidget()];
            }
        }
    }

    return SNew(SVerticalBox);
}

void FGLSEditorModule::OnTabClosed(TSharedRef<SDockTab> TabBeingClosed)
{
    if (IsValid(CreatedUMGWidget) && CreatedUMGWidget->IsValidLowLevel() && CreatedUMGWidget->IsValidLowLevelFast())
    {
        CreatedUMGWidget->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty);
        CreatedUMGWidget = nullptr;
    }
}

void FGLSEditorModule::ShutdownModule()
{
}

void FGLSEditorModule::OpenTabClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(GLSTabName);
}

void FGLSEditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
    Builder.AddMenuEntry(FGLSCommands::Get().OpenPluginWindow);
}

#undef IMAGE_BRUSH

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGLSEditorModule, GLSEditor);