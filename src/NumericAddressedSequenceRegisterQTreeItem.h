#ifndef NUMERICADDRESSEDSEQUENCEREGISTERQTREEITEM_H
#define NUMERICADDRESSEDSEQUENCEREGISTERQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"
#include <mtca4u/Device.h>

class NumericAddressedSequenceRegisterQTreeItem : public DeviceElementQTreeItem {

public:

  NumericAddressedSequenceRegisterQTreeItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, mtca4u::TwoDRegisterAccessor<double> & twoDRegisterAccessor, unsigned int channelNo, QTreeWidgetItem * parent, RegisterPropertiesWidget * propertiesWidget);
  virtual void readData();
  virtual void writeData();
  virtual void updateRegisterProperties();

private: // methods

private: // member fields
  mtca4u::TwoDRegisterAccessor<double> & twoDRegisterAccessor_;
  unsigned int channelNo_;
  RegisterPropertiesWidget * propertiesWidget_;
  RegisterPropertiesWidget::RegisterProperties * properties_;
  mtca4u::FixedPointConverter * fixedPointConverter_;
};

#endif // NUMERICADDRESSEDSEQUENCEREGISTERQTREEITEM_H