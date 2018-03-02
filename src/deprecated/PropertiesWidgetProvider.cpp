#include "PropertiesWidgetProvider.h"

PropertiesWidgetProvider::PropertiesWidgetProvider() {}

void PropertiesWidgetProvider::registerWidget(const int type,
                                              PropertiesWidget *widget,
                                              int page) {
  registeredWidgets_[type] = widget;
  registeredPages_[type] = page;
}

void PropertiesWidgetProvider::registerWidget(DeviceElementDataType type,
                                              PropertiesWidget *widget,
                                              int page) {
  const int typeInt = static_cast<int>(type);
  registerWidget(typeInt, widget, page);
}

PropertiesWidget *PropertiesWidgetProvider::get(const int type) {
  currentWidget_ = registeredWidgets_[type];
  return registeredWidgets_[type];
}

int PropertiesWidgetProvider::pageOf(const int type) {
  return registeredPages_[type];
}

PropertiesWidget *PropertiesWidgetProvider::getCurrentWidget() {
  return currentWidget_;
}