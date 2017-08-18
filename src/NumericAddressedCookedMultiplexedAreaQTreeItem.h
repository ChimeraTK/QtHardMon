#ifndef NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H
#define NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"
#include <mtca4u/Device.h>          // mtca4u::Device
#include <mtca4u/RegisterInfoMap.h> // mtca4u::RegisterInfoMap::RegisterInfo

class NumericAddressedCookedMultiplexedAreaQTreeItem
    : public DeviceElementQTreeItem {

public:
  NumericAddressedCookedMultiplexedAreaQTreeItem(
      mtca4u::Device &device,
      boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget *parent,
      PropertiesWidgetProvider &propertiesWidgetProvider);
  virtual void readData();
  virtual void writeData();
  virtual void updateRegisterProperties();

private: // member fields
  mtca4u::TwoDRegisterAccessor<double> twoDRegisterAccessor_;
  int bar_;
  int address_;
  int size_;
  std::vector<std::string> name_;
};

#endif // NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H