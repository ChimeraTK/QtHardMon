#ifndef MODULEQTREEITEM_H
#define MODULEQTREEITEM_H

#include "DeviceElementQTreeItem.h"

class ModuleQTreeItem : public DeviceElementQTreeItem {

public:

  ModuleQTreeItem(const QString & text, QTreeWidget * parent);

  ModuleQTreeItem(const QString & text, QTreeWidgetItem * parent);

  virtual void read();
  virtual void write();
  virtual void updateRegisterProperties();
};

#endif // MODULEQTREEITEM_H