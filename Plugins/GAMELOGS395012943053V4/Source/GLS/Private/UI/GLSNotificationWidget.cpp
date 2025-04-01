// Copyright (c) 2024 Fergius Engineering.

#include "GLSNotificationWidget.h"

#include <Components/TextBlock.h>
#include <Dom/JsonObject.h>
#include <HttpModule.h>
#include <Interfaces/IPluginManager.h>
#include <PlatformHttp.h>
#include <Serialization/JsonReader.h>
#include <Serialization/JsonSerializer.h>

#include "GLSSettings.h"

void UGLSNotificationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetVisibility(ESlateVisibility::Collapsed);

    if (auto Settings = UGLSSettings::Get())
    {
        if (Settings->bEnableNotification)
        {
            CheckForUpdates();
        }
    }
}

void UGLSNotificationWidget::CheckForUpdates()
{
    const FString Url = TEXT("https://gls-fergius.github.io/gls-updates/latest_version.json");

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UGLSNotificationWidget::OnCheckForUpdatesResponse);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->ProcessRequest();
}

bool IsVersionNewer(const FString& CurrentVersion, const FString& NewVersion)
{
    auto CleanVersion = [](const FString& Version) -> FString
    {
        int32 PlusIndex;
        if (Version.FindChar(TEXT('+'), PlusIndex))
        {
            return Version.Left(PlusIndex);
        }
        return Version;
    };

    FString CleanCurrentVersion = CleanVersion(CurrentVersion);
    FString CleanNewVersion = CleanVersion(NewVersion);

    TArray<FString> CurrentParts, NewParts;
    CleanCurrentVersion.ParseIntoArray(CurrentParts, TEXT("."), true);
    CleanNewVersion.ParseIntoArray(NewParts, TEXT("."), true);

    int32 MaxParts = FMath::Max(CurrentParts.Num(), NewParts.Num());

    for (int32 i = 0; i < MaxParts; i++)
    {
        int32 CurrentPart = (i < CurrentParts.Num()) ? FCString::Atoi(*CurrentParts[i]) : 0;
        int32 NewPart = (i < NewParts.Num()) ? FCString::Atoi(*NewParts[i]) : 0;

        if (NewPart > CurrentPart)
        {
            return true;
        }
        else if (NewPart < CurrentPart)
        {
            return false;
        }
    }

    return false;
}

void UGLSNotificationWidget::OnCheckForUpdatesResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || Response->GetResponseCode() != 200)
    {
        return;
    }

    auto Plugin = IPluginManager::Get().FindPlugin("GameLogsSystem");
    if (!Plugin)
    {
        return;
    }

    const FString CurrentVersion = Plugin->GetDescriptor().VersionName;

    const FString ResponseContent = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return;
    }

    const TArray<TSharedPtr<FJsonValue>> Versions = JsonObject->GetArrayField(TEXT("versions"));
    TArray<TPair<FString, FString>> Updates;

    for (const TSharedPtr<FJsonValue>& VersionEntry : Versions)
    {
        TSharedPtr<FJsonObject> VersionObject = VersionEntry->AsObject();
        if (!VersionObject.IsValid())
        {
            continue;
        }

        FString Version = VersionObject->GetStringField(TEXT("version"));
        FString Changelog = VersionObject->GetStringField(TEXT("changelog"));

        if (IsVersionNewer(CurrentVersion, Version))
        {
            Updates.Add(TPair<FString, FString>(Version, Changelog));
        }
    }

    if (Updates.Num() == 0)
    {
        return;
    }

    Updates.Sort([](const TPair<FString, FString>& A, const TPair<FString, FString>& B)
        {
            auto ParseVersion = [](const FString& Version) -> TArray<int32>
            {
                TArray<FString> Parts;
                Version.ParseIntoArray(Parts, TEXT("."), true);

                TArray<int32> NumericParts;
                for (const FString& Part : Parts)
                {
                    NumericParts.Add(FCString::Atoi(*Part));
                }

                return NumericParts;
            };

            TArray<int32> VersionA = ParseVersion(A.Key);
            TArray<int32> VersionB = ParseVersion(B.Key);

            int32 MaxParts = FMath::Max(VersionA.Num(), VersionB.Num());

            for (int32 i = 0; i < MaxParts; i++)
            {
                int32 PartA = (i < VersionA.Num()) ? VersionA[i] : 0;
                int32 PartB = (i < VersionB.Num()) ? VersionB[i] : 0;

                if (PartA != PartB)
                {
                    return PartA > PartB;
                }
            }

            return false;
        });

    FString LatestVersion = Updates[0].Key;
    FString Message = FString::Printf(TEXT("The current version of the plugin %s is outdated, please update plugin to %s version.\n\n"), *CurrentVersion, *LatestVersion);

    for (const TPair<FString, FString>& Update : Updates)
    {
        Message += FString::Printf(TEXT("- %s: %s\n"), *Update.Key, *Update.Value);
    }
    Message += TEXT("\nTo update the plugin, open the Epic Games Launcher, navigate to the Installed Plugins section, and apply the update.");

    SetToolTipText(FText::FromString(Message));

    SetVisibility(ESlateVisibility::Visible);
}
