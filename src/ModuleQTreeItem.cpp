#include "ModuleQTreeItem.h"
#include "Exceptions.h"
#include "RegisterPropertiesWidget.h"

ModuleQTreeItem::ModuleQTreeItem(const QString & text, QTreeWidget * parent,PropertiesWidgetProvider & propertiesWidgetProvider)
: DeviceElementQTreeItem(text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent, propertiesWidgetProvider),
  properties_(new RegisterPropertiesWidget::RegisterProperties())
{
}

ModuleQTreeItem::ModuleQTreeItem(const QString & text, QTreeWidgetItem * parent, PropertiesWidgetProvider & propertiesWidgetProvider)
: DeviceElementQTreeItem(text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent, propertiesWidgetProvider),
  properties_(new RegisterPropertiesWidget::RegisterProperties())
{
}

void ModuleQTreeItem::readData() {
  throw InvalidOperationException(
      "You cannot read from a module. Select a register.");
}

void ModuleQTreeItem::writeData() {
  throw InvalidOperationException(
      "You cannot write to a module. Select a register.");    
}

void ModuleQTreeItem::updateRegisterProperties() {
  dynamic_cast<RegisterPropertiesWidget*>(getPropertiesWidget())->setRegisterProperties(*properties_);
}