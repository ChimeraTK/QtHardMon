#ifndef NUMERICADDRESSEDREGISTERQTREEITEM_H
#define NUMERICADDRESSEDREGISTERQTREEITEM_H

#include "DeviceElementQTreeItem.h"
#include <mtca4u/Device.h> // mtca4u::Device
#include <mtca4u/RegisterInfoMap.h> // mtca4u::RegisterInfoMap::RegisterInfo

class NumericAddressedRegisterQTreeItem : public DeviceElementQTreeItem {

public:

  NumericAddressedRegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, double * transactionVariable);
  virtual void read();
  virtual void write();
  virtual void updateRegisterProperties();

private: // methods
  QTreeWidgetItem * assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * treeWidget);
  std::string getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo);

private: // member fields
  double * transactionVariable_;
  mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor_;
  
};

#endif // NUMERICADDRESSEDREGISTERQTREEITEM_H