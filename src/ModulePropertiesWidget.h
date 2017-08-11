#ifndef MODULEPROPERTIESWIDGET_H
#define MODULEPROPERTIESWIDGET_H

#include "PropertiesWidget.h"
#include "ui_ModulePropertiesWidget.h"
#include "CustomDelegates.h"

#include <mtca4u/FixedPointConverter.h>

namespace Ui {
class ModulePropertiesWidget;
}

class ModulePropertiesWidget : public PropertiesWidget
{
    Q_OBJECT

public:
    explicit ModulePropertiesWidget(QWidget *parent);
    ~ModulePropertiesWidget();
    
    virtual void clearFields();
    virtual void setSize(int nOfElements, int size = 0);
    virtual void setNames(std::vector<std::string> components);
    virtual void setFixedPointInfo(int width, int fracBits, int signBit);
    virtual void setAddress(int bar, int address);

public:
    Ui::ModulePropertiesWidget *ui;
    friend class QtHardMon;
    friend class PlotWindow;
};

#endif // MODULEPROPERTIESWIDGET_H
