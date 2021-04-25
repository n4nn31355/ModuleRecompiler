// Copyright n4nn31355. All Rights Reserved.

#include "ModuleRecompiler.h"

#include "ModuleRecompilerCommands.h"
#include "ModuleRecompilerSettings.h"
#include "ModuleRecompilerSettingsDetails.h"
#include "ModuleRecompilerStyle.h"

#include "IHotReload.h"
#include "ISettingsModule.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName ModuleRecompilerTabName("ModuleRecompiler");

#define LOCTEXT_NAMESPACE "FModuleRecompilerModule"

void
FModuleRecompilerModule::StartupModule()
{
  RegisterSettings();

  FModuleRecompilerStyle::Initialize();
  FModuleRecompilerStyle::ReloadTextures();

  FModuleRecompilerCommands::Register();

  PluginCommands = MakeShareable(new FUICommandList);

  PluginCommands->MapAction(
    FModuleRecompilerCommands::Get().PluginAction,
    FExecuteAction::CreateRaw(
      this, &FModuleRecompilerModule::PluginButtonClicked),
    FCanExecuteAction());

  UToolMenus::RegisterStartupCallback(
    FSimpleMulticastDelegate::FDelegate::CreateRaw(
      this, &FModuleRecompilerModule::RegisterMenus));

  RegisterLogging();
}

void
FModuleRecompilerModule::ShutdownModule()
{
  UnregisterSettings();

  UToolMenus::UnRegisterStartupCallback(this);

  UToolMenus::UnregisterOwner(this);

  FModuleRecompilerStyle::Shutdown();

  FModuleRecompilerCommands::Unregister();

  UnregisterLogging();
}

void
FModuleRecompilerModule::PluginButtonClicked()
{
  const UModuleRecompilerSettings* Settings =
    GetDefault<UModuleRecompilerSettings>();

  IHotReloadInterface& HotReloadSupport =
    FModuleManager::LoadModuleChecked<IHotReloadInterface>("HotReload");

  LogListing->AddMessage(
    TSharedRef<FTokenizedMessage>(FTokenizedMessage::Create(
      EMessageSeverity::Info,
      FText::Format(
        LOCTEXT(
          "RecompilationStarted",
          "Recompilation started: recompiling {0} modules"),
        Settings->ModuleNames.Num()))));

  for (const auto& ModuleName : Settings->ModuleNames) {
    if (!FModuleManager::Get().ModuleExists(*ModuleName)) {
      LogListing->AddMessage(
        TSharedRef<FTokenizedMessage>(FTokenizedMessage::Create(
          EMessageSeverity::Warning,
          FText::Format(
            LOCTEXT(
              "RecompilationFailed_ModuleDoesNotExist",
              "Recompilation skipped: module '{0}' doesn't exist"),
            FText::FromString(ModuleName)))));
      continue;
    }

    const bool bRecompileSucceeded = HotReloadSupport.RecompileModule(
      FName(*ModuleName),
      *GLog,
      ERecompileModuleFlags::ReloadAfterRecompile
        | ERecompileModuleFlags::FailIfGeneratedCodeChanges);

    FText CompilationStatusText =
      bRecompileSucceeded ? LOCTEXT("RecompilationStatus_Succeed", "SUCCEED")
                          : LOCTEXT("RecompilationStatus_Failed", "FAILED");

    LogListing->AddMessage(
      TSharedRef<FTokenizedMessage>(FTokenizedMessage::Create(
        bRecompileSucceeded ? EMessageSeverity::Info : EMessageSeverity::Error,
        FText::Format(
          LOCTEXT(
            "RecompilationStatus_Module",
            "Recompilation of module '{0}' {1}. More information is available "
            "in the 'Compiler Log'"),
          FText::FromString(ModuleName),
          CompilationStatusText))));
  }
}

void
FModuleRecompilerModule::RegisterMenus()
{
  // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
  FToolMenuOwnerScoped OwnerScoped(this);

  {
    UToolMenu* Menu =
      UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
    {
      FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
      Section.AddMenuEntryWithCommandList(
        FModuleRecompilerCommands::Get().PluginAction, PluginCommands);
    }
  }

  {
    UToolMenu* ToolbarMenu =
      UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
    {
      FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
      {
        FToolMenuEntry& Entry =
          Section.AddEntry(FToolMenuEntry::InitToolBarButton(
            FModuleRecompilerCommands::Get().PluginAction));
        Entry.SetCommandList(PluginCommands);
      }
    }
  }
}

void
FModuleRecompilerModule::RegisterSettings()
{
  ISettingsModule* SettingsModule =
    FModuleManager::GetModulePtr<ISettingsModule>("Settings");
  if (SettingsModule != nullptr) {
    SettingsModule->RegisterSettings(
      "Project",
      "Plugins",
      "Module Recompiler",
      LOCTEXT("ModuleRecompilerSettingsName", "Module Recompiler"),
      LOCTEXT(
        "ModuleRecompilerSettingsDescription",
        "Settings for Module Recompiler plugin"),
      GetMutableDefault<UModuleRecompilerSettings>());

    FPropertyEditorModule& PropertyModule =
      FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(
      UModuleRecompilerSettings::StaticClass()->GetFName(),
      FOnGetDetailCustomizationInstance::CreateStatic(
        &FModuleRecompilerSettingsDetails::MakeInstance));
  }
}

void
FModuleRecompilerModule::UnregisterSettings()
{
  ISettingsModule* SettingsModule =
    FModuleManager::GetModulePtr<ISettingsModule>("Settings");
  if (SettingsModule != nullptr) {
    SettingsModule->UnregisterSettings(
      "Project", "Plugins", "Module Recompiler");
  }
}

void
FModuleRecompilerModule::RegisterLogging()
{
  FMessageLogModule& MessageLogModule =
    FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
  FMessageLogInitializationOptions MessageLogOptions;
  MessageLogOptions.bAllowClear = true;
  MessageLogModule.RegisterLogListing(
    LogListingName,
    LOCTEXT("ModuleRecompiler", "Module Recompiler"),
    MessageLogOptions);
  LogListing = MessageLogModule.GetLogListing(LogListingName);
}

void
FModuleRecompilerModule::UnregisterLogging()
{
  FMessageLogModule& MessageLogModule =
    FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
  MessageLogModule.UnregisterLogListing(LogListingName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FModuleRecompilerModule, ModuleRecompiler)
