#ifndef MODULEQTREEITEM_H
#define MODULEQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"

class ModuleQTreeItem : public DeviceElementQTreeItem {

public:
  ModuleQTreeItem(const QString &text, QTreeWidget *parent,
                  PropertiesWidgetProvider &propertiesWidgetProvider);

  ModuleQTreeItem(const QString &text, QTreeWidgetItem *parent,
                  PropertiesWidgetProvider &propertiesWidgetProvider);

  virtual void readData(mtca4u::Device &device);
  virtual void writeData(mtca4u::Device &device);
  virtual void updateRegisterProperties(mtca4u::Device &device);

};

#endif // MODULEQTREEITEM_H
