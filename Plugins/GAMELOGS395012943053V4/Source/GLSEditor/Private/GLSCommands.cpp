// Copyright (c) 2024 Fergius Engineering.

#include "GLSCommands.h"

#define LOCTEXT_NAMESPACE "FGLSModule"

void FGLSCommands::RegisterCommands()
{
    UI_COMMAND(OpenPluginWindow, "Game Logs System", "Open game logs system editor", EUserInterfaceActionType::Button, FInputChord(EKeys::G, true, false, true, false));
}

#undef LOCTEXT_NAMESPACE
