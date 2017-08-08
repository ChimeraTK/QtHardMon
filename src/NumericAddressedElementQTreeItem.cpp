#include "NumericAddressedElementQTreeItem.h"
#include "Exceptions.h"
#include "ModuleQTreeItem.h"

NumericAddressedElementQTreeItem::NumericAddressedElementQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(QString(getRegisterName(registerInfo).c_str()),
  static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType), 
  assignToModuleItem(registerInfo, parent, propertiesWidget))
{
}

QTreeWidgetItem * NumericAddressedElementQTreeItem::assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * treeWidget, RegisterPropertiesWidget * propertiesWidget) {
  
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
      moduleItem = new ModuleQTreeItem(QString(moduleName.c_str()), treeWidget, propertiesWidget);
    } else {
      moduleItem = moduleList.front();
    }

    return moduleItem;
}

std::string NumericAddressedElementQTreeItem::getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  return registerInfo->getRegisterName().getComponents().back();
}