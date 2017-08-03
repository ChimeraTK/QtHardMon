#include "NumericAddressedRegisterQTreeItem.h"
#include "Exceptions.h"
#include "ModuleQTreeItem.h"

// FIXME: the assignment of text is done on initializer list by using dynamic_cast - this might throw.
// Move to constructor body and throw grafefully.
NumericAddressedRegisterQTreeItem::NumericAddressedRegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, double * transactionVariable)
: DeviceElementQTreeItem(QString(getRegisterName(registerInfo).c_str()),
  static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType), 
  assignToModuleItem(registerInfo, parent)),
  transactionVariable_(transactionVariable),
  oneDRegisterAccessor_(device.getOneDRegisterAccessor<double>(registerInfo->getRegisterName()))
{   
}

void NumericAddressedRegisterQTreeItem::read() {
  oneDRegisterAccessor_.read();
  *transactionVariable_ = oneDRegisterAccessor_[0];
}

void NumericAddressedRegisterQTreeItem::write() {
     oneDRegisterAccessor_[0] = *transactionVariable_;
     oneDRegisterAccessor_.write();
}

void NumericAddressedRegisterQTreeItem::updateRegisterProperties() {

}

QTreeWidgetItem * NumericAddressedRegisterQTreeItem::assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * treeWidget) {
  
    std::vector<std::string> registerPathComponents = registerInfo->getRegisterName().getComponents();
    std::string moduleName = registerPathComponents.front();
    
    // FIXME: There is a high chance, that there are always two components.
    if (registerPathComponents.size() < 2 || moduleName.empty()) {
      moduleName = NO_MODULE_NAME_STRING;
    }

    QList<QTreeWidgetItem *> moduleList = 
      treeWidget->findItems(QString(moduleName.c_str()), Qt::MatchExactly);

    QTreeWidgetItem * moduleItem;
    if (moduleList.empty()) {
      moduleItem = new ModuleQTreeItem(QString(moduleName.c_str()), treeWidget);
    } else {
      moduleItem = moduleList.front();
    }

    return moduleItem;
}

std::string NumericAddressedRegisterQTreeItem::getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  return registerInfo->getRegisterName().getComponents().back();
}