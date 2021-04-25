// Copyright n4nn31355. All Rights Reserved.

#include "ModuleRecompilerSettingsDetails.h"

#include "ModuleRecompilerSettings.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Views/SListView.h"

#define LOCTEXT_NAMESPACE "FModuleRecompilerModule"

TSharedRef<IDetailCustomization>
FModuleRecompilerSettingsDetails::MakeInstance()
{
  return MakeShareable(new FModuleRecompilerSettingsDetails);
}

void
FModuleRecompilerSettingsDetails::CustomizeDetails(
  IDetailLayoutBuilder& DetailLayout)
{
  IDetailCategoryBuilder& CategoryBuilder =
    DetailLayout.EditCategory("Modules", FText::GetEmpty());

  TSharedPtr<SVerticalBox> VBox;

  CategoryBuilder
    .AddCustomRow(
      LOCTEXT("ModuleRecompilerModules", "ModuleRecompiler Modules"))
    .WholeRowContent()[SAssignNew(VBox, SVerticalBox)];

  const TSharedRef<SCheckBox> FilterSelectedCheckbox =
    SNew(SCheckBox)
      .ToolTipText(
        LOCTEXT("ShowOnlySelectedTooltip", "Show only selected modules"))
      .IsChecked(
        this, &FModuleRecompilerSettingsDetails::GetFilterSelectedState)
      .OnCheckStateChanged(
        this, &FModuleRecompilerSettingsDetails::OnFilterSelectedChanged)
        [SNew(STextBlock).Text(LOCTEXT("Selected", "Selected"))];

  const TSharedRef<SCheckBox> FilterProjectCheckbox =
    SNew(SCheckBox)
      .ToolTipText(LOCTEXT(
        "ShowOnlyProjectTooltip", "Show only modules in the project directory"))
      .IsChecked(this, &FModuleRecompilerSettingsDetails::GetFilterProjectState)
      .OnCheckStateChanged(
        this, &FModuleRecompilerSettingsDetails::OnFilterProjectChanged)
        [SNew(STextBlock).Text(LOCTEXT("Project", "Project"))];

  // Filter buttons and searchbar
  VBox->AddSlot().AutoHeight().Padding(4.0f)
    [SNew(SHorizontalBox)
     + SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[FilterSelectedCheckbox]
     + SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[FilterProjectCheckbox]
     + SHorizontalBox::Slot().FillWidth(1.0f).Padding(
       2.0f)[SAssignNew(FilterSearchBox, SSearchBox)
               .ToolTipText(
                 LOCTEXT("FilterSearchboxTooltip", "Filter module by name"))
               .OnTextChanged(
                 this,
                 &FModuleRecompilerSettingsDetails::OnFilterTextChanged)]];

  // List of modules
  VBox->AddSlot().MaxHeight(400.0f).FillHeight(
    1.0f)[SAssignNew(ModuleListView, SModuleListView)
            .ListItemsSource(&ModuleListItems)
            .OnGenerateRow(
              this, &FModuleRecompilerSettingsDetails::OnGenerateModuleListRow)
            .HeaderRow(
              SNew(SHeaderRow)
              + SHeaderRow::Column("ModuleRecompilationState")
                  .DefaultLabel(FText())
                  .FixedWidth(20.0f)
              + SHeaderRow::Column("ModuleName")
                  .DefaultLabel(LOCTEXT("ModuleName", "Module Name"))
                  .FixedWidth(200.0f)
              + SHeaderRow::Column("ModulePath")
                  .DefaultLabel(LOCTEXT("ModulePath", "Path"))
                  .FillWidth(1.0f))];

  UpdateModuleListItems();
}

TSharedRef<ITableRow>
FModuleRecompilerSettingsDetails::OnGenerateModuleListRow(
  FModuleListItem Item,
  const TSharedRef<STableViewBase>& OwnerTable)
{
  const UModuleRecompilerSettings* Settings =
    GetDefault<UModuleRecompilerSettings>();

  const TSharedRef<SCheckBox> ModuleStateCheckbox =
    SNew(SCheckBox)
      .ToolTipText(LOCTEXT(
        "ModuleStateCheckboxTooltip", "Select module for recompilation"))
      .IsChecked(
        Settings->ModuleNames.Contains(*Item->Name) ? ECheckBoxState::Checked
                                                    : ECheckBoxState::Unchecked)
      .OnCheckStateChanged_Lambda([Item](ECheckBoxState State) {
        UModuleRecompilerSettings* ModuleRecompilerSettings =
          GetMutableDefault<UModuleRecompilerSettings>();
        if (
          State == ECheckBoxState::Checked
          && !ModuleRecompilerSettings->ModuleNames.Contains(*Item->Name)) {
          ModuleRecompilerSettings->ModuleNames.Add(*Item->Name);
        } else if (
          State == ECheckBoxState::Unchecked
          && ModuleRecompilerSettings->ModuleNames.Contains(*Item->Name)) {
          ModuleRecompilerSettings->ModuleNames.Remove(*Item->Name);
        }
        ModuleRecompilerSettings->SaveConfig();
      });

  return SNew(STableRow<FModuleListItem>, OwnerTable)
    [SNew(SHorizontalBox)
     + SHorizontalBox::Slot().AutoWidth().Padding(2.0f).VAlign(
       EVerticalAlignment::VAlign_Center)[ModuleStateCheckbox]
     + SHorizontalBox::Slot().MaxWidth(200.0f).Padding(2.0f).VAlign(
       EVerticalAlignment::VAlign_Center)[SNew(STextBlock)
                                            .ToolTipText(
                                              FText::FromName(*Item->Name))
                                            .Text(FText::FromName(*Item->Name))]
     + SHorizontalBox::Slot().FillWidth(1.0f).Padding(2.0f).VAlign(
       EVerticalAlignment::VAlign_Center)
         [SNew(STextBlock)
            .ToolTipText(FText::FromName(*Item->FilePath))
            .Text(FText::FromName(*Item->FilePath))]];
}

void
FModuleRecompilerSettingsDetails::OnFilterTextChanged(const FText& FilterText)
{
  UpdateModuleListItems();
}

ECheckBoxState
FModuleRecompilerSettingsDetails::GetFilterSelectedState() const
{
  return bFilterSelected ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
FModuleRecompilerSettingsDetails::OnFilterSelectedChanged(
  ECheckBoxState NewState)
{
  bFilterSelected = (NewState == ECheckBoxState::Checked);
  UpdateModuleListItems();
}

ECheckBoxState
FModuleRecompilerSettingsDetails::GetFilterProjectState() const
{
  return bFilterProject ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
FModuleRecompilerSettingsDetails::OnFilterProjectChanged(
  ECheckBoxState NewState)
{
  bFilterProject = (NewState == ECheckBoxState::Checked);
  UpdateModuleListItems();
}

void
FModuleRecompilerSettingsDetails::UpdateModuleListItems()
{
  ModuleListItems.Reset();

  TArray<FModuleStatus> ModuleStatuses;
  FModuleManager::Get().QueryModules(ModuleStatuses);

  const UModuleRecompilerSettings* ModuleRecompilerSettings =
    GetDefault<UModuleRecompilerSettings>();

  for (const auto& Module : ModuleStatuses) {
    FString ModuleName = *Module.Name;

    FString FilterStr = FilterSearchBox->GetText().ToString();
    FilterStr.TrimStartAndEndInline();
    if (!FilterStr.IsEmpty() && !Module.Name.Contains(FilterStr)) {
      continue;
    }
    if (
      bFilterSelected
      && !ModuleRecompilerSettings->ModuleNames.Contains(Module.Name)) {
      continue;
    }
    if (bFilterProject && !Module.FilePath.StartsWith(FPaths::ProjectDir())) {
      continue;
    }

    ModuleListItems.Add(MakeShared<FModuleStatus>(Module));
  }

  ModuleListItems.Sort([](const FModuleListItem& A, const FModuleListItem& B) {
    return A->Name.Compare(*B->Name) < 0;
  });

  if (ModuleListView.IsValid()) {
    ModuleListView->RequestListRefresh();
  }
}

#undef LOCTEXT_NAMESPACE
