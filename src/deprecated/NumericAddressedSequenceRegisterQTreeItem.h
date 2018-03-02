#ifndef NUMERICADDRESSEDSEQUENCEREGISTERQTREEITEM_H
#define NUMERICADDRESSEDSEQUENCEREGISTERQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"
#include <mtca4u/Device.h>

class NumericAddressedSequenceRegisterQTreeItem
    : public DeviceElementQTreeItem {

public:
  NumericAddressedSequenceRegisterQTreeItem(
      boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
      mtca4u::TwoDRegisterAccessor<double> &twoDRegisterAccessor,
      unsigned int channelNo, QTreeWidgetItem *parent,
      PropertiesWidgetProvider &propertiesWidgetProvider);
  virtual void updateRegisterProperties(mtca4u::Device &device);
  virtual void readData(mtca4u::Device &device);
  virtual void writeData(mtca4u::Device &device);

private: // methods
private: // member fields
  mtca4u::TwoDRegisterAccessor<double> &twoDRegisterAccessor_;
  unsigned int channelNo_;
  int bar_;
  int address_;
  int size_;
  int width_;
  int fracBits_;
  int signFlag_;
  std::vector<std::string> name_;
  mtca4u::FixedPointConverter *fixedPointConverter_;
};

#endif // NUMERICADDRESSEDSEQUENCEREGISTERQTREEITEM_H
