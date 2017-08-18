#ifndef NUMERICADDRESSEDREGISTERQTREEITEM_H
#define NUMERICADDRESSEDREGISTERQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"
#include <mtca4u/Device.h>              // mtca4u::Device
#include <mtca4u/FixedPointConverter.h> // mtca4u::FixedPointConverter
#include <mtca4u/RegisterInfoMap.h>     // mtca4u::RegisterInfoMap::RegisterInfo

class NumericAddressedRegisterQTreeItem : public DeviceElementQTreeItem {

public:
  NumericAddressedRegisterQTreeItem(
      mtca4u::Device &device,
      boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget *parent,
      PropertiesWidgetProvider &propertiesWidgetProvider);
  virtual void readData();
  virtual void writeData();
  virtual void updateRegisterProperties();

private: // methods
private: // member fields
  mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor_;
  int bar_;
  int address_;
  int size_;
  int width_;
  int fracBits_;
  int signFlag_;
  std::vector<std::string> name_;
  mtca4u::FixedPointConverter *fixedPointConverter_;
};

#endif // NUMERICADDRESSEDREGISTERQTREEITEM_H