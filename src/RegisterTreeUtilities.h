#ifndef REGISTERTREEUTILITIES_H
#define REGISTERTREEUTILITIES_H

#include <mtca4u/Device.h>
#include <qtreewidget.h>
#include "PropertiesWidgetProvider.h"

class RegisterTreeUtilities {

public:
    static QTreeWidgetItem * assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * treeWidget, PropertiesWidgetProvider & propertiesWidgetProvider);
    static std::string getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo);
};

#endif // REGISTERTREEUTILITIES_H