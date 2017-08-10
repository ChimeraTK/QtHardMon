#ifndef NUMERICADDRESSEDREGISTERQTREEITEM_H
#define NUMERICADDRESSEDREGISTERQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include <mtca4u/Device.h> // mtca4u::Device
#include <mtca4u/RegisterInfoMap.h> // mtca4u::RegisterInfoMap::RegisterInfo
#include <mtca4u/FixedPointConverter.h> // mtca4u::FixedPointConverter
#include "RegisterPropertiesWidget.h"

class NumericAddressedRegisterQTreeItem : public DeviceElementQTreeItem {

public:

  NumericAddressedRegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget);
  virtual void readData();
  virtual void writeData();
  virtual void updateRegisterProperties();

private: // methods

private: // member fields
  mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor_;
  RegisterPropertiesWidget * propertiesWidget_;
  RegisterPropertiesWidget::RegisterProperties * properties_;
  mtca4u::FixedPointConverter * fixedPointConverter_;
};

#endif // NUMERICADDRESSEDREGISTERQTREEITEM_H