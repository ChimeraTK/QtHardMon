#ifndef NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H
#define NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include <mtca4u/Device.h> // mtca4u::Device
#include <mtca4u/RegisterInfoMap.h> // mtca4u::RegisterInfoMap::RegisterInfo
#include "RegisterPropertiesWidget.h"

class NumericAddressedCookedMultiplexedAreaQTreeItem : public DeviceElementQTreeItem {

public:

  NumericAddressedCookedMultiplexedAreaQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget);
  virtual void read();
  virtual void write();
  virtual void updateRegisterProperties();

private: // member fields
  mtca4u::TwoDRegisterAccessor<double> twoDRegisterAccessor_;
  RegisterPropertiesWidget * propertiesWidget_;
  RegisterPropertiesWidget::RegisterProperties * properties_;
};

#endif // NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H