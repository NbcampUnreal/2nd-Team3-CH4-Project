// Copyright (c) 2024 Fergius Engineering.

#pragma once

#include <Framework/Application/IInputProcessor.h>
#include <Framework/Application/SlateApplication.h>
#include <GameFramework/PlayerInput.h>
#include <Input/Events.h>

/**
 * @class FGLSInputProcessor
 * @brief Class for handling user input with support for key combination detection to open the GLS overlay.
 * 
 * This class implements the IInputProcessor interface, enabling the processing of keyboard events 
 * and tracking the state of keys, including modifiers (Shift, Ctrl, Alt, Cmd). The primary purpose is 
 * to provide an input handling mechanism that is independent of input modes (UI Only, Game Only).
 * 
 * Input mappings for displaying the overlay are configured in `UGLSSettings::OverlayInputMappings`.
 * 
 * Key Features:
 * - Handles key press and release events (`HandleKeyDownEvent` and `HandleKeyUpEvent`).
 * - Tracks key states, including modifier keys, via an internal map `KeyStates`.
 * - Supports detection of key combinations based on `FInputActionKeyMapping` configurations.
 * - Can be easily extended to check multiple combinations.
 * 
 * Notes:
 * - Does not consume key events by default (can be changed via the return values of `HandleKeyDownEvent` 
 *   and `HandleKeyUpEvent`).
 * - Requires explicit registration with `FSlateApplication` to function.
 * 
 */
class FGLSInputProcessor : public IInputProcessor
{
public:
    FGLSInputProcessor();

    DECLARE_EVENT(FGLSInputProcessor, FGLSInputOverlayEvent)
    FGLSInputOverlayEvent OnInputOverlayPressed;
    FGLSInputOverlayEvent OnInputOverlayReleased;

    /** Input key mappings to create the GLS overlay */
    TArray<FInputActionKeyMapping> OverlayInputMappings;

protected:
    virtual const TCHAR* GetDebugName() const override { return TEXT("FGLSInputProcessor"); }

    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

    virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

    virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

    virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override{};

private:
    TMap<FKey, bool> KeyStates;

    TMap<int32, FVector2D> ActiveTouches; 

    void CheckKeyPressedCombination(const FKeyEvent& InKeyEvent);

    void CheckKeyReleasedCombination(const FKeyEvent& InKeyEvent);

    int32 GetTouchIndexFromKey(const FKey& Key) const;

    bool IsMatchingInput(const FInputActionKeyMapping& Mapping) const;
};
