#ifndef NUMERICADDRESSEDELEMENTQTREEITEM_H
#define NUMERICADDRESSEDELEMENTQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include <mtca4u/Device.h> // mtca4u::Device
#include <mtca4u/RegisterInfoMap.h> // mtca4u::RegisterInfoMap::RegisterInfo
#include <mtca4u/FixedPointConverter.h> // mtca4u::FixedPointConverter
#include "RegisterPropertiesWidget.h"

class NumericAddressedElementQTreeItem : public DeviceElementQTreeItem {

public:

  NumericAddressedElementQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget);
  virtual void read() = 0;
  virtual void write() = 0;
  virtual void updateRegisterProperties();

protected: // methods
  QTreeWidgetItem * assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * treeWidget);
  std::string getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo);

protected: // member fields
  RegisterPropertiesWidget * propertiesWidget_;
  RegisterPropertiesWidget::RegisterProperties * properties_;
  mtca4u::FixedPointConverter * fixedPointConverter_;
};

#endif // NUMERICADDRESSEDELEMENTQTREEITEM_H