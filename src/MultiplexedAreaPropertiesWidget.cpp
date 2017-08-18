#include "MultiplexedAreaPropertiesWidget.h"
#include "Constants.h"

MultiplexedAreaPropertiesWidget::MultiplexedAreaPropertiesWidget(QWidget *parent) :
    PropertiesWidget(parent),
    ui(new Ui::MultiplexedAreaPropertiesWidget)
{
    ui->setupUi(this);
    clearFields();
}

MultiplexedAreaPropertiesWidget::~MultiplexedAreaPropertiesWidget()
{
    delete ui;
}

void MultiplexedAreaPropertiesWidget::clearFields() {
    ui->registerNameDisplay     ->setText("");
    ui->moduleDisplay           ->setText("");
    ui->registerBarDisplay      ->setText("");
    ui->registerNElementsDisplay->setText("");
    ui->registerAddressDisplay  ->setText("");
    ui->registerSizeDisplay     ->setText("");
}


void MultiplexedAreaPropertiesWidget::setSize(int nOfElements, int size) {
    ui->registerNElementsDisplay->setText(QString::number(nOfElements));
    ui->registerSizeDisplay->setText(QString::number(size));
}

void MultiplexedAreaPropertiesWidget::setNames(std::vector<std::string> components) {
    std::string moduleName;
    if (components.size() >= 2) {
        for (int i = 0; i < components.size() - 1; ++i) {
            moduleName += "/" + components[i];
        }
        moduleName = moduleName.substr(1);
    } else 
        moduleName = "";

    ui->registerNameDisplay->setText(components.back().c_str());
    ui->moduleDisplay->setText(moduleName.c_str());
}

void MultiplexedAreaPropertiesWidget::setFixedPointInfo(int width, int fracBits, int signBit) {
}

void MultiplexedAreaPropertiesWidget::setAddress(int bar, int address) {
    ui->registerBarDisplay->setText(QString::number(bar));
    ui->registerAddressDisplay->setText(QString::number(address));
}