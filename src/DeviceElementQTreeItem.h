#ifndef DEVICEELEMENTQTREEITEM_H
#define DEVICEELEMENTQTREEITEM_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "Constants.h"
#include <mtca4u/Device.h>              // mtca4u::Device


class DeviceElementQTreeItem : public QTreeWidgetItem {

public:
  DeviceElementQTreeItem(QTreeWidget *parent,
                         QString const & name,
                         boost::shared_ptr<mtca4u::RegisterInfo> registerInfo = nullptr);
  
  DeviceElementQTreeItem(QTreeWidgetItem *parent,
                         QString const & name,
                         boost::shared_ptr<mtca4u::RegisterInfo> registerInfo = nullptr);

  bool operator<(const QTreeWidgetItem &rhs) const;

  boost::shared_ptr<mtca4u::RegisterInfo> getRegisterInfo();

protected:
  boost::shared_ptr<mtca4u::RegisterInfo> registerInfo_;

};

#endif // DEVICEELEMENTQTREEITEM_H
