#ifndef PROPERTIESWIDGETPROVIDER_H
#define PROPERTIESWIDGETPROVIDER_H

#include <map>
#include "PropertiesWidget.h"
#include "Constants.h"

class PropertiesWidgetProvider {

private:
    std::map<const int, PropertiesWidget *> registeredWidgets_;
    std::map<const int, int> registeredPages_;

public:
    PropertiesWidgetProvider();
    void registerWidget(const int type, PropertiesWidget * widget, int page);
    void registerWidget(DeviceElementDataType type, PropertiesWidget * widget, int page);
    PropertiesWidget * get(const int type);
    int pageOf(const int type);
};

#endif // PROPERTIESWIDGETPROVIDER_H