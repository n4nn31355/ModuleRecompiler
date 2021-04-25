// Copyright n4nn31355. All Rights Reserved.

using UnrealBuildTool;

public class ModuleRecompiler : ModuleRules
{
  public ModuleRecompiler(ReadOnlyTargetRules Target)
    : base(Target)
  {
    PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

    PublicDependencyModuleNames.AddRange(new string[] {
      "Core",
    });

    PrivateDependencyModuleNames.AddRange(new string[] {
      "Projects",
      "InputCore",
      "UnrealEd",
      "ToolMenus",
      "CoreUObject",
      "Engine",
      "Slate",
      "SlateCore",
      "HotReload",
      "MessageLog",
    });
  }
}
