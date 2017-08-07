#ifndef MODULEQTREEITEM_H
#define MODULEQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"

class ModuleQTreeItem : public DeviceElementQTreeItem {

public:

  ModuleQTreeItem(const QString & text, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget);

  ModuleQTreeItem(const QString & text, QTreeWidgetItem * parent, RegisterPropertiesWidget * propertiesWidget);

  virtual void read();
  virtual void write();
  virtual void updateRegisterProperties();

private: // member fields
  RegisterPropertiesWidget * propertiesWidget_;
  RegisterPropertiesWidget::RegisterProperties * properties_;
};

#endif // MODULEQTREEITEM_H