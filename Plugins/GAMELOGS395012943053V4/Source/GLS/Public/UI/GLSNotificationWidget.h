// Copyright (c) 2024 Fergius Engineering.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Interfaces/IHttpRequest.h>
#include <Interfaces/IHttpResponse.h>
#include <UObject/ObjectPtr.h>

#include "GLSNotificationWidget.generated.h"

UCLASS(Abstract)
class GLS_API UGLSNotificationWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    void NativeConstruct() override;

    void CheckForUpdates();

    void OnCheckForUpdatesResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};