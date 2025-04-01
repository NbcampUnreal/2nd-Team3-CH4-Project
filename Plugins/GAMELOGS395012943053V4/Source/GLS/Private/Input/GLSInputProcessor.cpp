#include "GLSInputProcessor.h"

FGLSInputProcessor::FGLSInputProcessor()
{
    KeyStates.Add(EKeys::LeftShift, false);
    KeyStates.Add(EKeys::RightShift, false);
    KeyStates.Add(EKeys::LeftControl, false);
    KeyStates.Add(EKeys::RightControl, false);
    KeyStates.Add(EKeys::LeftAlt, false);
    KeyStates.Add(EKeys::RightAlt, false);
    KeyStates.Add(EKeys::LeftCommand, false);
    KeyStates.Add(EKeys::RightCommand, false);
}

bool FGLSInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    //UE_LOG(LogTemp, Log, TEXT("HandleKeyDownEvent: %s"), *InKeyEvent.GetKey().ToString());

    const FKey PressedKey = InKeyEvent.GetKey();
    KeyStates.Add(PressedKey, true);

    CheckKeyPressedCombination(InKeyEvent);

    return false;
}

bool FGLSInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    //UE_LOG(LogTemp, Log, TEXT("HandleKeyUpEvent: %s"), *InKeyEvent.GetKey().ToString());

    const FKey ReleasedKey = InKeyEvent.GetKey();
    KeyStates.Add(ReleasedKey, false);

    CheckKeyReleasedCombination(InKeyEvent);

    return false;
}

bool FGLSInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsTouchEvent())
    {
        ActiveTouches.Add(MouseEvent.GetPointerIndex(), MouseEvent.GetScreenSpacePosition());
        //UE_LOG(LogTemp, Log, TEXT("Touch started: %d"), MouseEvent.GetPointerIndex());

        for (const FInputActionKeyMapping& InputActionKeyMapping : OverlayInputMappings)
        {
            if (IsMatchingInput(InputActionKeyMapping))
            {
                //UE_LOG(LogTemp, Log, TEXT("HandleMouseButtonDownEvent: Action matched: %s"), *InputActionKeyMapping.ActionName.ToString());
                OnInputOverlayPressed.Broadcast();
            }
        }
    }
    return false;
}

bool FGLSInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsTouchEvent())
    {
        ActiveTouches.Remove(MouseEvent.GetPointerIndex());
        //UE_LOG(LogTemp, Log, TEXT("Touch ended: %d"), MouseEvent.GetPointerIndex());

        OnInputOverlayReleased.Broadcast();
    }
    return false;
}

void FGLSInputProcessor::CheckKeyPressedCombination(const FKeyEvent& InKeyEvent)
{
    //UE_LOG(LogTemp, Log, TEXT("CheckKeyPressedCombination"));
    for (const FInputActionKeyMapping& InputActionKeyMapping : OverlayInputMappings)
    {
        const bool bShift = KeyStates[EKeys::LeftShift] || KeyStates[EKeys::RightShift];
        const bool bCtrl = KeyStates[EKeys::LeftControl] || KeyStates[EKeys::RightControl];
        const bool bAlt = KeyStates[EKeys::LeftAlt] || KeyStates[EKeys::RightAlt];
        const bool bCmd = KeyStates[EKeys::LeftCommand] || KeyStates[EKeys::RightCommand];

        if (InputActionKeyMapping.Key == InKeyEvent.GetKey() &&
            InputActionKeyMapping.bShift == bShift &&
            InputActionKeyMapping.bCtrl == bCtrl &&
            InputActionKeyMapping.bAlt == bAlt &&
            InputActionKeyMapping.bCmd == bCmd)
        {
            //UE_LOG(LogTemp, Log, TEXT("CheckKeyPressedCombination: %s"), *InputActionKeyMapping.ActionName.ToString());
            OnInputOverlayPressed.Broadcast();
            break;
        }
    }
}

void FGLSInputProcessor::CheckKeyReleasedCombination(const FKeyEvent& InKeyEvent)
{
    //UE_LOG(LogTemp, Log, TEXT("CheckKeyPressedCombination"));
    for (const FInputActionKeyMapping& InputActionKeyMapping : OverlayInputMappings)
    {
        if (InputActionKeyMapping.Key == InKeyEvent.GetKey())
        {
            //UE_LOG(LogTemp, Log, TEXT("CheckKeyReleasedCombination: %s"), *InputActionKeyMapping.ActionName.ToString());
            OnInputOverlayReleased.Broadcast();
        }
    }
}

int32 FGLSInputProcessor::GetTouchIndexFromKey(const FKey& Key) const
{
    for (int32 Index = 0; Index < EKeys::NUM_TOUCH_KEYS; ++Index)
    {
        if (EKeys::TouchKeys[Index] == Key)
        {
            return Index + 1;
        }
    }
    return 0;
}

bool FGLSInputProcessor::IsMatchingInput(const FInputActionKeyMapping& Mapping) const
{
    if (Mapping.Key.IsTouch())
    {
        int32 RequiredTouches = GetTouchIndexFromKey(Mapping.Key);
        return ActiveTouches.Num() == RequiredTouches;
    }
    return false;
}
