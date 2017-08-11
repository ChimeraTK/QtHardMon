#include "PropertiesWidgetProvider.h"

PropertiesWidgetProvider::PropertiesWidgetProvider() {}

void PropertiesWidgetProvider::registerWidget(const int type, PropertiesWidget * widget) {
    registeredWidgets_[type] = widget;
}

void PropertiesWidgetProvider::registerWidget(DeviceElementDataType type, PropertiesWidget * widget) {
    const int typeInt = static_cast<int>(type);
    registerWidget(typeInt, widget);
}

PropertiesWidget * PropertiesWidgetProvider::get(const int type) {
    return registeredWidgets_[type];
}