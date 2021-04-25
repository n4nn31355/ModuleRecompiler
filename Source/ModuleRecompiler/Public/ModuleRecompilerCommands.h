// Copyright n4nn31355. All Rights Reserved.

#pragma once

#include "ModuleRecompilerStyle.h"

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FModuleRecompilerCommands : public TCommands<FModuleRecompilerCommands>
{
public:
  FModuleRecompilerCommands()
    : TCommands<FModuleRecompilerCommands>(
      TEXT("ModuleRecompiler"),
      NSLOCTEXT("Contexts", "ModuleRecompiler", "ModuleRecompiler Plugin"),
      NAME_None,
      FModuleRecompilerStyle::GetStyleSetName())
  {}

  // TCommands<> interface
  virtual void RegisterCommands() override;

public:
  TSharedPtr<FUICommandInfo> PluginAction;
};
