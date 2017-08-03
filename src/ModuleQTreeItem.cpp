#include "ModuleQTreeItem.h"
#include "Exceptions.h"

ModuleQTreeItem::ModuleQTreeItem(const QString & text, QTreeWidget * parent)
: DeviceElementQTreeItem(text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent) 
{   
}

ModuleQTreeItem::ModuleQTreeItem(const QString & text, QTreeWidgetItem * parent)
: DeviceElementQTreeItem(text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent) 
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

}