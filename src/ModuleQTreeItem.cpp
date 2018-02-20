#include "ModuleQTreeItem.h"
#include "Exceptions.h"
#include "RegisterPropertiesWidget.h"

ModuleQTreeItem::ModuleQTreeItem(
    const QString &text, QTreeWidget *parent,
    PropertiesWidgetProvider &propertiesWidgetProvider)
    : DeviceElementQTreeItem(
          text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent,
          propertiesWidgetProvider) {}

ModuleQTreeItem::ModuleQTreeItem(
    const QString &text, QTreeWidgetItem *parent,
    PropertiesWidgetProvider &propertiesWidgetProvider)
    : DeviceElementQTreeItem(
          text, static_cast<int>(DeviceElementDataType::ModuleDataType), parent,
          propertiesWidgetProvider) {}


void ModuleQTreeItem::readData(mtca4u::Device &device) {
	throw InvalidOperationException(
	      "You cannot read from a module. Select a register.");
}

void ModuleQTreeItem::writeData(mtca4u::Device &device) {
	throw InvalidOperationException(
	      "You cannot write to a module. Select a register.");
}


void ModuleQTreeItem::updateRegisterProperties(mtca4u::Device &device) {
  getPropertiesWidget()->clearFields();
  getPropertiesWidget()->setNames({text(0).toStdString()});
  getPropertiesWidget()->setSize(childCount());
}
