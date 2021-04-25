// Copyright n4nn31355. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "Widgets/Views/STableViewBase.h"

class FModuleRecompilerSettingsDetails : public IDetailCustomization
{
public:
  static TSharedRef<IDetailCustomization> MakeInstance();

  virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

private:
  typedef TSharedPtr<const FModuleStatus> FModuleListItem;
  typedef SListView<FModuleListItem> SModuleListView;

  TSharedPtr<SModuleListView> ModuleListView;
  TSharedRef<ITableRow> OnGenerateModuleListRow(
    FModuleListItem Item,
    const TSharedRef<STableViewBase>& OwnerTable);

  TSharedPtr<SSearchBox> FilterSearchBox;
  void OnFilterTextChanged(const FText& FilterText);

  bool bFilterSelected = false;
  ECheckBoxState GetFilterSelectedState() const;
  void OnFilterSelectedChanged(ECheckBoxState NewState);

  bool bFilterProject = false;
  ECheckBoxState GetFilterProjectState() const;
  void OnFilterProjectChanged(ECheckBoxState NewState);

  void UpdateModuleListItems();
  TArray<FModuleListItem> ModuleListItems;
};
