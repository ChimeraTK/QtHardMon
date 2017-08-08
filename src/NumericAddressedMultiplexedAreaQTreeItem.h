#ifndef NUMERICADDRESSEDMULTIPLEXEDAREAQTREEITEM_H
#define NUMERICADDRESSEDMULTIPLEXEDAREAQTREEITEM_H

#include "NumericAddressedElementQTreeItem.h"

class NumericAddressedMultiplexedAreaQTreeItem : public NumericAddressedElementQTreeItem {

public:

  NumericAddressedMultiplexedAreaQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, const mtca4u::RegisterCatalogue & catalogue, mtca4u::RegisterCatalogue::const_iterator firstSequenceItem, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget);
  virtual void read();
  virtual void write();
  virtual void updateRegisterProperties();

private: // methods
  std::string getTrimmedRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo);
private: // member fields
  mtca4u::TwoDRegisterAccessor<double> twoDRegisterAccessor_;
  RegisterPropertiesWidget * propertiesWidget_;
  RegisterPropertiesWidget::RegisterProperties * properties_;
};

#endif // NUMERICADDRESSEDMULTIPLEXEDAREAQTREEITEM_H