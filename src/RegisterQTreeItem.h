#ifndef REGISTERQTREEITEM_H
#define REGISTERQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"
#include <mtca4u/Device.h>              // mtca4u::Device
#include <mtca4u/FixedPointConverter.h> // mtca4u::FixedPointConverter
#include <mtca4u/RegisterInfoMap.h>     // mtca4u::RegisterInfoMap::RegisterInfo

class RegisterQTreeItem : public DeviceElementQTreeItem {

public:
  RegisterQTreeItem(mtca4u::Device &device,
                    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
                    QTreeWidget *parent,
                    PropertiesWidgetProvider &propertiesWidgetProvider);
  virtual void readData(mtca4u::Device &device);
  virtual void writeData(mtca4u::Device &device);
  virtual void updateRegisterProperties(mtca4u::Device &device);

private: // methods
private: // member fields
  mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor_;
  std::vector<std::string> name_;
};

#endif // REGISTERQTREEITEM_H
