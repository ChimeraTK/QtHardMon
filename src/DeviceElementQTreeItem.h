#ifndef DEVICEELEMENTQTREEITEM_H
#define DEVICEELEMENTQTREEITEM_H

#include <qtreewidget.h>

class DeviceElementQTreeItem : public QTreeWidgetItem {

public:

  DeviceElementQTreeItem(const QString& text, const int type, 
                        QTreeWidget* parent);

  DeviceElementQTreeItem(const QString & text, const int type, 
                        QTreeWidgetItem * parent);

  virtual void read() = 0;
  virtual void write() = 0;
  virtual void updateRegisterProperties() = 0;
};

#endif // DEVICEELEMENTQTREEITEM_H