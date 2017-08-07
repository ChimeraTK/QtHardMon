#include "RegisterPropertiesWidget.h"


RegisterPropertiesWidget::RegisterPropertiesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPropertiesWidget)
{
    ui->setupUi(this);

    clearProperties();
}

RegisterPropertiesWidget::~RegisterPropertiesWidget()
{
    delete ui;
}

void RegisterPropertiesWidget::clearProperties() {
    RegisterProperties properties;
    setRegisterProperties(properties);
}

void RegisterPropertiesWidget::setRegisterProperties(RegisterProperties properties) {
    ui->registerNameDisplay     ->setText(properties.registerName);
    ui->moduleDisplay           ->setText(properties.moduleName);
    ui->registerBarDisplay      ->setText(properties.bar);
    ui->registerNElementsDisplay->setText(properties.nOfElements);
    ui->registerAddressDisplay  ->setText(properties.address);
    ui->registerSizeDisplay     ->setText(properties.size);
    ui->registerWidthDisplay    ->setText(properties.width);
    ui->registerFracBitsDisplay ->setText(properties.fracBits);
    ui->registeSignBitDisplay   ->setText(properties.signBit);
}

RegisterPropertiesWidget::RegisterProperties::RegisterProperties(
QString RegisterName,
QString ModuleName,
QString Bar,
QString Address,
QString NOfElements,
QString Size,
QString Width,
QString FracBits,
QString SignBit
) 
        : registerName(RegisterName),
            moduleName(ModuleName),
            bar(Bar),
            address(Address),
            nOfElements(NOfElements),
            size(Size),
            width(Width),
            fracBits(FracBits),
            signBit(SignBit) {}
