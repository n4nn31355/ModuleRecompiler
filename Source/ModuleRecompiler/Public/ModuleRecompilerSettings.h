// Copyright n4nn31355. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "ModuleRecompilerSettings.generated.h"

UCLASS(config = EditorPerProjectUserSettings, defaultconfig)
class MODULERECOMPILER_API UModuleRecompilerSettings : public UObject
{
  GENERATED_BODY()

public:
  /** List of module names to recompile */
  UPROPERTY(Config, EditAnywhere, Category = "Settings")
  TArray<FString> ModuleNames;
};
