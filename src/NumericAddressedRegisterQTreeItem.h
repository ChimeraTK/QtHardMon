#ifndef NUMERICADDRESSEDREGISTERQTREEITEM_H
#define NUMERICADDRESSEDREGISTERQTREEITEM_H

#include "NumericAddressedElementQTreeItem.h"

class NumericAddressedRegisterQTreeItem : public NumericAddressedElementQTreeItem {

public:

  NumericAddressedRegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget);
  virtual void read();
  virtual void write();

private: // methods

private: // member fields
  mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor_;
};

#endif // NUMERICADDRESSEDREGISTERQTREEITEM_H