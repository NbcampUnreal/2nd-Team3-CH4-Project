// Copyright (c) 2024 Fergius Engineering.

#pragma once

#include <Engine/Engine.h>

#include "GLSOutputDevice.h"
#include "GLSSettings.h"
#include "GLSSubsystem.h"
#include "GLSTypes.h"
#include "GLSUtils.h"

#if defined(GLS_ENABLED) && GLS_ENABLED

/*
 * Logs a message with the specified category and verbosity level to both the console output and the GLS overlay.
 * Example: GLS_LOG_STRING_WITH_TAGS(WorldContext, LogTemp, Display, TEXT("My test log message"), TArray<FString>({TEXT("my-tag-1"), TEXT("my-tag-2")}));
 * Example: GLS_LOG_STRING_WITH_TAGS(WorldContext, LogTemp, Display, FString::Printf(TEXT("My test log message with param %d"), 12345), TArray<FString>({TEXT("my-tag-1"), TEXT("my-tag-2")}));
 */
#define GLS_LOG_STRING_WITH_TAGS(_Context, _Category, _Verbosity, _String, _LogTags)                                                \
    {                                                                                                                               \
        const FString _gls_String = _String;                                                                                        \
        const auto* _gls_Settings = UGLSSettings::Get();                                                                            \
        if (FGLSUtils::IsGLSEnabled() && _gls_Settings)                                                                             \
        {                                                                                                                           \
            static const FString _gls_Category(#_Category);                                                                         \
            static const FString _gls_FunctionName(__FUNCTION__);                                                                   \
            TArray<FString> _gls_LogTags = _LogTags;                                                                                \
            if (_gls_Settings->DefaultLoggingTagTypes & (uint8)EGLSTagsDefaultTypes::Function && !_gls_FunctionName.IsEmpty())      \
                _gls_LogTags.Add(FString::Format(TEXT("{0}"), {_gls_FunctionName}));                                                \
            FGLSOutputDevice::SerializeForGLS(_Context, _gls_LogTags, _gls_String, ELogVerbosity::Type::_Verbosity, _gls_Category); \
        }                                                                                                                           \
        if (auto Device = FGLSOutputDevice::Get())                                                                                  \
        {                                                                                                                           \
            Device->AddExcludeEntry(_gls_String);                                                                                   \
        }                                                                                                                           \
        UE_LOG(_Category, _Verbosity, TEXT("%s"), *_gls_String);                                                                    \
        if (auto Subsystem = UGLSSubsystem::Get())                                                                                  \
        {                                                                                                                           \
            Subsystem->ForceProcessNewLogEntries();                                                                                 \
        }                                                                                                                           \
    }

/*
 * Logs a message with the specified category and verbosity level to both the console output and the GLS overlay.
 * Example: GLS_LOG_WITH_TAGS(WorldContext, LogTemp, Display, TArray<FString>({TEXT("my-tag-1"), TEXT("my-tag-2")}), TEXT("My test log message with param %d"), 12345);
 */
#define GLS_LOG_WITH_TAGS(_Context, _Category, _Verbosity, _LogTags, _Format, ...)                                    \
    {                                                                                                                 \
        GLS_LOG_STRING_WITH_TAGS(_Context, _Category, _Verbosity, FString::Printf(_Format, ##__VA_ARGS__), _LogTags); \
    }

/*
 * Logs a message with the specified category and verbosity level to both the console output and the GLS overlay.
 * Example:  GLS_LOG(WorldContext, LogTemp, Warning, TEXT("My test log message with param %d"), 12345);
 */
#define GLS_LOG_CONTEXT(_Context, _Category, _Verbosity, _Format, ...)                                 \
    {                                                                                                  \
        GLS_LOG_WITH_TAGS(_Context, _Category, _Verbosity, TArray<FString>{}, _Format, ##__VA_ARGS__); \
    }

/*
 * Logs a message with the specified category and verbosity level to both the console output and the GLS overlay.
 * The log automatically associates the current world context for proper context management, ensuring the log is displayed in the correct game world and context.
 * NOTE: This macro utilizes functions from UObject, ensuring proper context and object validation.
 * NOTE: This macro will not work within non-object functions or static functions, as it requires a valid UObject context for proper operation.
 * NOTE: If you are unable to use the macro due to the limitations mentioned above, please use the standard UE_LOG macro as an alternative for logging messages.
 * Example: GLS_LOG(MyCategory, Warning, TEXT("My test log message with param %d"), 12345);
 */
#define GLS_LOG(_Category, _Verbosity, _Format, ...)                               \
    {                                                                              \
        const UObject* _Context = nullptr;                                         \
        if (TIsDerivedFrom<std::remove_pointer_t<decltype(this)>, UObject>::Value) \
        {                                                                          \
            _Context = reinterpret_cast<const UObject*>(this);                     \
        }                                                                          \
        GLS_LOG_CONTEXT(_Context, _Category, _Verbosity, _Format, ##__VA_ARGS__);  \
    }

/*
 * Logs a message with the specified category and verbosity level to both the console output and the GLS overlay.
 * The log automatically associates the current world context for proper context management, ensuring the log is displayed in the correct game world and context.
 * NOTE: This macro utilizes functions from UObject, ensuring proper context and object validation.
 * NOTE: This macro will not work within non-object functions or static functions, as it requires a valid UObject context for proper operation.
 * NOTE: If you are unable to use the macro due to the limitations mentioned above, please use the standard UE_LOG macro as an alternative for logging messages.
 * Example: GLS_LOG_STRING(LogTemp, Warning, TEXT("My test log message"));
 * Example: GLS_LOG_STRING(LogTemp, Warning, FString::Printf(TEXT("My test log message with param %d"), 12345));
 */
#define GLS_LOG_STRING(_Category, _Verbosity, _String)                 \
    {                                                                  \
        GLS_LOG(_Category, _Verbosity, TEXT("%s"), *FString(_String)); \
    }

/*
 * Logs a message with the specified category and verbosity level to both the console output and the GLS overlay.
 * Example: GLS_LOG_STRING_CONTEXT(LogTemp, Warning, TEXT("My test log message"));
 * Example: GLS_LOG_STRING_CONTEXT(LogTemp, Warning, FString::Printf(TEXT("My test log message with param %d"), 12345));
 */
#define GLS_LOG_STRING_CONTEXT(_Context, _Category, _Verbosity, _String)                 \
    {                                                                                    \
        GLS_LOG_CONTEXT(_Context, _Category, _Verbosity, TEXT("%s"), *FString(_String)); \
    }

#else
#define GLS_LOG_STRING_WITH_TAGS(_Context, _Category, _Verbosity, _String, _LogTags) UE_LOG(_Category, _Verbosity, TEXT("%s"), *FString(_String));
#define GLS_LOG_WITH_TAGS(_Context, _Category, _Verbosity, _LogTags, _Format, ...) UE_LOG(_Category, _Verbosity, _Format, ##__VA_ARGS__);
#define GLS_LOG_CONTEXT(_Context, _Category, _Verbosity, _Format, ...) UE_LOG(_Category, _Verbosity, _Format, ##__VA_ARGS__);
#define GLS_LOG(_Category, _Verbosity, _Format, ...) UE_LOG(_Category, _Verbosity, _Format, ##__VA_ARGS__);
#define GLS_LOG_STRING_CONTEXT(_Context, _Category, _Verbosity, _String) UE_LOG(_Category, _Verbosity, TEXT("%s"), *FString(_String));
#define GLS_LOG_STRING(_Category, _Verbosity, _String) UE_LOG(_Category, _Verbosity, TEXT("%s"), *FString(_String));
#endif