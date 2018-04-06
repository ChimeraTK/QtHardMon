#include "RegisterTreeUtilities.h"
#include "DeviceElementQTreeItem.h"

using namespace mtca4u;

QTreeWidgetItem * getNodeFromTreeWidget(std::string const & nodeName, QTreeWidget *treeWidget){
  auto nodeList = treeWidget->findItems(nodeName.c_str(), Qt::MatchExactly);

  if (nodeList.empty()) {
    return nullptr;
  } else {
    return nodeList.front();
  }
}

QTreeWidgetItem * getNodeFromTreeWidgetItem(std::string const & nodeName, QTreeWidgetItem *parentTreeWidgetItem){
  for (int i = 0; i < parentTreeWidgetItem->childCount(); ++i) {
    auto childNode=parentTreeWidgetItem->child(i);
    if ( childNode->text(0) == QString(nodeName.c_str()) ) {
      return childNode;
    }
  }
  return nullptr;
}


QTreeWidgetItem *RegisterTreeUtilities::getDeepestBranchNode(
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
    QTreeWidget *treeWidget){

  std::vector<std::string> registerPathComponents =
      registerInfo->getRegisterName().getComponents();
  std::string moduleName = registerPathComponents.front();

  if (registerPathComponents.size() < 2 || moduleName.empty()) {
    return nullptr;
  }

  QList<QTreeWidgetItem *> moduleList =
      treeWidget->findItems(QString(moduleName.c_str()), Qt::MatchExactly);

  QTreeWidgetItem *moduleItem;
  if (moduleList.empty()) {
    moduleItem = new DeviceElementQTreeItem(treeWidget, moduleName.c_str());
  } else {
    moduleItem = moduleList.front();
  }

  // Calling a method, that will handle submodules assignment and generation
  // or, in case there are none, will simply return moduleItem.
  return getDeepestBranchNode(registerInfo, moduleItem);
}

std::string RegisterTreeUtilities::getRegisterName(
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  return registerInfo->getRegisterName().getComponents().back();
}

QTreeWidgetItem *RegisterTreeUtilities::getDeepestBranchNode(
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
    QTreeWidgetItem *parentModuleItem, unsigned int depth) {
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
      moduleItem = new DeviceElementQTreeItem(
        parentModuleItem,
        registerPathComponents.at(depth + 1).c_str());
    }
    return getDeepestBranchNode(registerInfo, moduleItem, depth + 1);
  } else {
    return parentModuleItem;
  }
}
