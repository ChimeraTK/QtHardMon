#include "RegisterPropertiesWidget.h"
#include "Constants.h"



RegisterPropertiesWidget::RegisterPropertiesWidget(QWidget *parent) :
    PropertiesWidget(parent),
    ui(new Ui::RegisterPropertiesWidget)
{
    ui->setupUi(this);
    clearFields();

    connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)),
    this, SLOT( updateTable(int, int) ) );
    
    setTableWidget(ui->valuesTableWidget);


}

void RegisterPropertiesWidget::updateTable(int row, int column) {
    updateTableEntries(row, column);
}

RegisterPropertiesWidget::~RegisterPropertiesWidget()
{
    delete ui;
}

void RegisterPropertiesWidget::clearFields() {
    ui->registerNameDisplay     ->setText("");
    ui->moduleDisplay           ->setText("");
    ui->registerBarDisplay      ->setText("");
    ui->registerNElementsDisplay->setText("");
    ui->registerAddressDisplay  ->setText("");
    ui->registerSizeDisplay     ->setText("");
    ui->registerWidthDisplay    ->setText("");
    ui->registerFracBitsDisplay ->setText("");
    ui->registeSignBitDisplay   ->setText("");
}

void RegisterPropertiesWidget::setSize(int nOfElements, int size) {
    ui->registerNElementsDisplay->setText(QString::number(nOfElements));
    ui->registerSizeDisplay->setText(QString::number(size));
}

void RegisterPropertiesWidget::setNames(std::vector<std::string> components) {
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

void RegisterPropertiesWidget::setFixedPointInfo(int width, int fracBits, int signBit) {
    ui->registerWidthDisplay->setText(QString::number(width));
    ui->registerFracBitsDisplay->setText(QString::number(fracBits));
    ui->registeSignBitDisplay->setText(QString::number(signBit));
}

void RegisterPropertiesWidget::setAddress(int bar, int address) {
    ui->registerBarDisplay->setText(QString::number(bar));
    ui->registerAddressDisplay->setText(QString::number(address));
}