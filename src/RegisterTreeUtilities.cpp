#include "RegisterTreeUtilities.h"
#include "ModuleQTreeItem.h"

#define NO_MODULE_NAME_STRING "[No Module Name]"

QTreeWidgetItem * RegisterTreeUtilities::assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * treeWidget, PropertiesWidgetProvider & propertiesWidgetProvider) {
  
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
      moduleItem = new ModuleQTreeItem(QString(moduleName.c_str()), treeWidget, propertiesWidgetProvider);
    } else {
      moduleItem = moduleList.front();
    }

    return moduleItem;
}

std::string RegisterTreeUtilities::getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  return registerInfo->getRegisterName().getComponents().back();
}