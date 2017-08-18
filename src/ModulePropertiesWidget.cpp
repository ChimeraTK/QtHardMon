#include "ModulePropertiesWidget.h"

ModulePropertiesWidget::ModulePropertiesWidget(QWidget *parent) :
    PropertiesWidget(parent),
    ui(new Ui::ModulePropertiesWidget)
{
    ui->setupUi(this);

    clearFields();
}

ModulePropertiesWidget::~ModulePropertiesWidget()
{
    delete ui;
}

void ModulePropertiesWidget::clearFields() {
    ui->moduleDisplay->setText("");
    ui->registerNElementsDisplay->setText("");
}

void ModulePropertiesWidget::setSize(int nOfElements, int /* size */) {
    ui->registerNElementsDisplay->setText(QString::number(nOfElements));
}

void ModulePropertiesWidget::setNames(std::vector<std::string> components) {
    ui->moduleDisplay->setText(components.front().c_str());
}

void ModulePropertiesWidget::setFixedPointInfo(int /* width */, int /* fracBits */, int /* signBit */) {
    // FIXME: should throw, nothing should use that method
    // FIXME: change from part of the interface to mixin
}

void ModulePropertiesWidget::setAddress(int /* bar */, int /* address */) {
    // FIXME: should throw, nothing should use that method
    // FIXME: change from part of the interface to mixin
}