#include "ModulePropertiesWidget.h"

// The default maximum for the number of words in a register.
// This limits the number of rows in the valuesTableWidget to avoid a segmentation fault if too much
// memory is requested.
static const size_t DEFAULT_MAX_WORDS = 0x10000;

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

void ModulePropertiesWidget::setSize(int nOfElements, int size) {
    ui->registerNElementsDisplay->setText(QString::number(nOfElements));
}

void ModulePropertiesWidget::setNames(std::vector<std::string> components) {
    ui->moduleDisplay->setText(components.front().c_str());
}

void ModulePropertiesWidget::setFixedPointInfo(int width, int fracBits, int signBit) {
    // FIXME: should throw, nothing should use that method
    // FIXME: change from part of the interface to mixin
}

void ModulePropertiesWidget::setAddress(int bar, int address) {
    // FIXME: should throw, nothing should use that method
    // FIXME: change from part of the interface to mixin
}