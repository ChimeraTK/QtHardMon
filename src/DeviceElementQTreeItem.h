#ifndef DEVICEELEMENTQTREEITEM_H
#define DEVICEELEMENTQTREEITEM_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <ChimeraTK/Device.h> // ChimeraTK::Device

class DeviceElementQTreeItem : public QTreeWidgetItem {
 public:
  DeviceElementQTreeItem(QTreeWidget* parent, QString const& name,
      const ChimeraTK::RegisterInfo& registerInfo = ChimeraTK::RegisterInfo{nullptr});

  DeviceElementQTreeItem(QTreeWidgetItem* parent, QString const& name,
      const ChimeraTK::RegisterInfo& registerInfo = ChimeraTK::RegisterInfo{nullptr});

  bool operator<(const QTreeWidgetItem& rhs) const;

  const ChimeraTK::RegisterInfo& getRegisterInfo();

  // Returns the path of the object. Also works for tree items without register
  // info because the partial register path is obtained from the tree structure.
  ChimeraTK::RegisterPath getRegisterPath();

 protected:
  ChimeraTK::RegisterInfo registerInfo_;
};

#endif // DEVICEELEMENTQTREEITEM_H
