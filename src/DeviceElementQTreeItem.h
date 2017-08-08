#ifndef DEVICEELEMENTQTREEITEM_H
#define DEVICEELEMENTQTREEITEM_H

#include <qtreewidget.h>

#define NO_MODULE_NAME_STRING "[No Module Name]"

enum class DeviceElementDataType {
    ModuleDataType = QTreeWidgetItem::UserType + 1,
    NumAddressedRegisterDataType,
    MultiplexedAreaDataType,
    SequenceRegisterDataType,
    GenericRegisterDataType
};

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