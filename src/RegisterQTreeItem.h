#ifndef REGISTERQTREEITEM_H
#define REGISTERQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include <mtca4u/Device.h> // mtca4u::Device
#include <mtca4u/RegisterInfoMap.h> // mtca4u::RegisterInfoMap::RegisterInfo
#include <mtca4u/FixedPointConverter.h> // mtca4u::FixedPointConverter
#include "RegisterPropertiesWidget.h"

class RegisterQTreeItem : public DeviceElementQTreeItem {

public:

  RegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, PropertiesWidgetProvider & propertiesWidgetProvider);
  virtual void readData();
  virtual void writeData();
  virtual void updateRegisterProperties();

private: // methods

private: // member fields
  mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor_;
  std::vector<std::string> name_;
};

#endif // REGISTERQTREEITEM_H