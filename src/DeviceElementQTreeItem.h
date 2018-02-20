#ifndef DEVICEELEMENTQTREEITEM_H
#define DEVICEELEMENTQTREEITEM_H

#include <qtreewidget.h>

#include "Constants.h"
#include "PropertiesWidgetProvider.h"
#include <mtca4u/Device.h>              // mtca4u::Device


class DeviceElementQTreeItem : public QTreeWidgetItem {

public:
  DeviceElementQTreeItem(const QString &text, const int type,
                         QTreeWidget *parent,
                         PropertiesWidgetProvider &propertiesWidgetProvider);

  DeviceElementQTreeItem(const QString &text, const int type,
                         QTreeWidgetItem *parent,
                         PropertiesWidgetProvider &propertiesWidgetProvider);

  virtual bool operator<(const QTreeWidgetItem &rhs) const;

  virtual void readData(mtca4u::Device &device) = 0;
  virtual void writeData(mtca4u::Device &device) = 0;
  virtual void updateRegisterProperties(mtca4u::Device &device) = 0;



  virtual PropertiesWidget *getPropertiesWidget();

private:
  PropertiesWidgetProvider &propertiesWidgetProvider_;
protected:
  boost::shared_ptr<mtca4u::RegisterInfo> registerInfo_;

};

#endif // DEVICEELEMENTQTREEITEM_H
