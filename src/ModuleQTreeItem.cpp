#include "ModuleQTreeItem.h"
#include "Exceptions.h"

ModuleQTreeItem::ModuleQTreeItem(const QString & text, QTreeWidget * parent,RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent),
  propertiesWidget_(propertiesWidget),
  properties_(new RegisterPropertiesWidget::RegisterProperties())
{
}

ModuleQTreeItem::ModuleQTreeItem(const QString & text, QTreeWidgetItem * parent, RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent),
  propertiesWidget_(propertiesWidget),
  properties_(new RegisterPropertiesWidget::RegisterProperties())
{
}

void ModuleQTreeItem::read() {
  throw InvalidOperationException(
      "You cannot read from a module. Select a register.");
}

void ModuleQTreeItem::write() {
  throw InvalidOperationException(
      "You cannot write to a module. Select a register.");    
}

void ModuleQTreeItem::updateRegisterProperties() {
  propertiesWidget_->setRegisterProperties(*properties_);
}