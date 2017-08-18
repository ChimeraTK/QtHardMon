#ifndef DEVICEELEMENTQTREEITEM_H
#define DEVICEELEMENTQTREEITEM_H

#include <qtreewidget.h>

#include "Constants.h"
#include "PropertiesWidgetProvider.h"

class DeviceElementQTreeItem : public QTreeWidgetItem {

public:
  DeviceElementQTreeItem(const QString &text, const int type,
                         QTreeWidget *parent,
                         PropertiesWidgetProvider &propertiesWidgetProvider);

  DeviceElementQTreeItem(const QString &text, const int type,
                         QTreeWidgetItem *parent,
                         PropertiesWidgetProvider &propertiesWidgetProvider);

  virtual bool operator<(const QTreeWidgetItem &rhs) const;

  virtual void readData() = 0;
  virtual void writeData() = 0;
  virtual void updateRegisterProperties() = 0;

  virtual PropertiesWidget *getPropertiesWidget();

private:
  PropertiesWidgetProvider &propertiesWidgetProvider_;
};

#endif // DEVICEELEMENTQTREEITEM_H