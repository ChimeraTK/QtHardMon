#ifndef REGISTERPROPERTIESWIDGET_H
#define REGISTERPROPERTIESWIDGET_H

#include "PropertiesWidget.h"
#include "ui_RegisterPropertiesWidget.h"
#include "CustomDelegates.h"

#include "NumericDataTableMixin.h"

namespace Ui {
class RegisterPropertiesWidget;
}

class RegisterPropertiesWidget : public PropertiesWidget, public NumericDataTableMixin
{
    Q_OBJECT

public:
    struct RegisterProperties {

    QString registerName;
    QString moduleName;
    QString bar;
    QString address;
    QString nOfElements;
    QString size;
    QString width;
    QString fracBits;
    QString signBit;

    RegisterProperties(QString RegisterName = "",
QString ModuleName = "",
QString Bar = "",
QString Address = "",
QString NOfElements = "",
QString Size = "",
QString Width = "",
QString FracBits = "",
QString SignBit = "");
    };

public:
    explicit RegisterPropertiesWidget(QWidget *parent);
    ~RegisterPropertiesWidget();
    
    // PropertiesWidget interface implementation

    virtual void clearFields();
    virtual void setSize(int nOfElements, int nOfChannels);
    virtual void setNames(std::vector<std::string> components);
    virtual void setFixedPointInfo(int width, int fracBits, int signBit);
    virtual void setAddress(int bar, int address);

    void setRegisterProperties(RegisterProperties properties);




private slots:
    void updateTable( int row, int column );


public:
    Ui::RegisterPropertiesWidget *ui;

    friend class QtHardMon;
    friend class PlotWindow;
};

#endif // REGISTERPROPERTIESWIDGET_H
