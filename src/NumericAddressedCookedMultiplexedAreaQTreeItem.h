#ifndef NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H
#define NUMERICADDRESSEDCOOKEDMULTIPLEXEDAREAQTREEITEM_H

#include "NumericAddressedElementQTreeItem.h"

class NumericAddressedCookedMultiplexedAreaQTreeItem : public NumericAddressedElementQTreeItem {

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