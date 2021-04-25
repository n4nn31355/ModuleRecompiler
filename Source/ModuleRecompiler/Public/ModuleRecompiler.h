// Copyright n4nn31355. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IMessageLogListing.h"
#include "MessageLogModule.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FModuleRecompilerModule : public IModuleInterface
{
public:
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;

  void PluginButtonClicked();

private:
  void RegisterSettings();
  void UnregisterSettings();

  void RegisterLogging();
  void UnregisterLogging();

  void RegisterMenus();

  TSharedPtr<class IMessageLogListing> LogListing;
  const FName LogListingName = TEXT("Module Recompiler");

private:
  TSharedPtr<class FUICommandList> PluginCommands;
};
