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
    explicit RegisterPropertiesWidget(QWidget *parent);
    ~RegisterPropertiesWidget();
    
    void clearProperties();
    void setRegisterProperties(const std::string & registerName = "",
                               const std::string & moduleName = "", 
                               int * bar = nullptr, 
                               int * nOfElements = nullptr, 
                               int * address = nullptr, 
                               int * size = nullptr, 
                               int * width = nullptr, 
                               int * fracBits = nullptr, 
                               int * signBit = nullptr
                              );

private slots:

private:

private:
    Ui::RegisterPropertiesWidget *ui;

    friend class QtHardMon;
    friend class PlotWindow;
};

#endif // REGISTERPROPERTIESWIDGET_H
