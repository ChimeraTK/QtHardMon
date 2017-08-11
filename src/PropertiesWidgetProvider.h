#ifndef PROPERTIESWIDGETPROVIDER_H
#define PROPERTIESWIDGETPROVIDER_H

#include <map>
#include "PropertiesWidget.h"
#include "Constants.h"

class PropertiesWidgetProvider {

private:
    std::map<const int, PropertiesWidget *> registeredWidgets_;

public:
    PropertiesWidgetProvider();
    void registerWidget(const int type, PropertiesWidget * widget);
    void registerWidget(DeviceElementDataType type, PropertiesWidget * widget);
    PropertiesWidget * get(const int type);
};

#endif // PROPERTIESWIDGETPROVIDER_H