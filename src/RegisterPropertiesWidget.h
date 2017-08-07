#ifndef REGISTERPROPERTIESWIDGET_H
#define REGISTERPROPERTIESWIDGET_H

#include <QWidget>
#include "ui_RegisterPropertiesWidget.h"

namespace Ui {
class RegisterPropertiesWidget;
}

class RegisterPropertiesWidget : public QWidget
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
    
    void clearProperties();
    void setRegisterProperties(RegisterProperties properties);

private slots:

private:

private:
    Ui::RegisterPropertiesWidget *ui;

    friend class QtHardMon;
    friend class PlotWindow;
};

#endif // REGISTERPROPERTIESWIDGET_H
