#ifndef NUMERICADDRESSEDMULTIPLEXEDAREAQTREEITEM_H
#define NUMERICADDRESSEDMULTIPLEXEDAREAQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include <mtca4u/Device.h> // mtca4u::Device
#include <mtca4u/RegisterInfoMap.h> // mtca4u::RegisterInfoMap::RegisterInfo
#include "RegisterPropertiesWidget.h"

class NumericAddressedMultiplexedAreaQTreeItem : public DeviceElementQTreeItem {

public:

  NumericAddressedMultiplexedAreaQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, const mtca4u::RegisterCatalogue & catalogue, mtca4u::RegisterCatalogue::const_iterator & firstSequenceItem, QTreeWidget * parent, PropertiesWidgetProvider & propertiesWidgetProvider);
  virtual void readData();
  virtual void writeData();
  virtual void updateRegisterProperties();

private: // methods
  std::string getTrimmedRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo);
private: // member fields
  mtca4u::TwoDRegisterAccessor<double> twoDRegisterAccessor_;
  int bar_;
  int address_;
  int size_;
  std::vector<std::string> name_;
};

#endif // NUMERICADDRESSEDMULTIPLEXEDAREAQTREEITEM_H