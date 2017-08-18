#ifndef REGISTERTREEUTILITIES_H
#define REGISTERTREEUTILITIES_H

#include "PropertiesWidgetProvider.h"
#include <mtca4u/Device.h>
#include <qtreewidget.h>

class RegisterTreeUtilities {

public:
  static QTreeWidgetItem *
  assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
                     QTreeWidget *treeWidget,
                     PropertiesWidgetProvider &propertiesWidgetProvider);
  static QTreeWidgetItem *
  assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
                     QTreeWidgetItem *parentModuleItem,
                     PropertiesWidgetProvider &propertiesWidgetProvider,
                     unsigned int depth = 0);
  static std::string
  getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo);
};

#endif // REGISTERTREEUTILITIES_H