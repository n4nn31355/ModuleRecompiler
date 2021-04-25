// Copyright n4nn31355. All Rights Reserved.

#include "ModuleRecompilerCommands.h"

#define LOCTEXT_NAMESPACE "FModuleRecompilerModule"

void
FModuleRecompilerCommands::RegisterCommands()
{
  UI_COMMAND(
    PluginAction,
    "Recompile Modules",
    "Recompile Modules",
    EUserInterfaceActionType::Button,
    FInputGesture());
}

#undef LOCTEXT_NAMESPACE
