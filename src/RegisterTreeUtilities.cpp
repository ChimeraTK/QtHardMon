#include "RegisterTreeUtilities.h"
#include "ModuleQTreeItem.h"

#define NO_MODULE_NAME_STRING "[No Module Name]"

QTreeWidgetItem *RegisterTreeUtilities::assignToModuleItem(
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
    QTreeWidget *treeWidget,
    PropertiesWidgetProvider &propertiesWidgetProvider) {

  std::vector<std::string> registerPathComponents =
      registerInfo->getRegisterName().getComponents();
  std::string moduleName = registerPathComponents.front();

  // FIXME: There is a high chance, that there are always two components.
  if (registerPathComponents.size() < 2 || moduleName.empty()) {
    moduleName = NO_MODULE_NAME_STRING;
  }

  QList<QTreeWidgetItem *> moduleList =
      treeWidget->findItems(QString(moduleName.c_str()), Qt::MatchExactly);

  QTreeWidgetItem *moduleItem;
  if (moduleList.empty()) {
    moduleItem = new ModuleQTreeItem(QString(moduleName.c_str()), treeWidget,
                                     propertiesWidgetProvider);
  } else {
    moduleItem = moduleList.front();
  }

  // Calling a method, that will handle submodules assignment and generation
  // or, in case there are none, will simply return moduleItem.
  return assignToModuleItem(registerInfo, moduleItem, propertiesWidgetProvider);
}

std::string RegisterTreeUtilities::getRegisterName(
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  return registerInfo->getRegisterName().getComponents().back();
}

QTreeWidgetItem *RegisterTreeUtilities::assignToModuleItem(
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
    QTreeWidgetItem *parentModuleItem,
    PropertiesWidgetProvider &propertiesWidgetProvider, unsigned int depth) {
  std::vector<std::string> registerPathComponents =
      registerInfo->getRegisterName().getComponents();
  if (registerPathComponents.size() > depth + 2) {
    QTreeWidgetItem *moduleItem = nullptr;

    for (int i = 0; i < parentModuleItem->childCount(); ++i) {
      if (parentModuleItem->child(i)->text(0) ==
          QString(registerPathComponents.at(depth + 1).c_str())) {
        moduleItem = parentModuleItem->child(i);
        break;
      }
    }
    if (!moduleItem) {
      moduleItem = new ModuleQTreeItem(
          QString(registerPathComponents.at(depth + 1).c_str()),
          parentModuleItem, propertiesWidgetProvider);
    }
    return assignToModuleItem(registerInfo, moduleItem,
                              propertiesWidgetProvider, depth + 1);
  } else {
    return parentModuleItem;
  }
}