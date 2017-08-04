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
    setRegisterProperties();
}

void RegisterPropertiesWidget::setRegisterProperties(const std::string & registerName, const std::string & moduleName, int * bar, int * nOfElements, int * address, int * size, int * width, int * fracBits, int * signBit) {
    ui->registerNameDisplay     ->setText(!registerName.empty() ? QString(registerName.c_str()) : QString(""));
    ui->moduleDisplay           ->setText(!moduleName.empty()   ? QString(moduleName.c_str())   : QString(""));
    ui->registerBarDisplay      ->setText(bar                   ? QString::number(*bar)         : QString(""));
    ui->registerNElementsDisplay->setText(nOfElements           ? QString::number(*nOfElements) : QString(""));
    ui->registerAddressDisplay  ->setText(address               ? QString::number(*address)     : QString(""));
    ui->registerSizeDisplay     ->setText(size                  ? QString::number(*size)        : QString(""));
    ui->registerWidthDisplay    ->setText(width                 ? QString::number(*width)       : QString(""));
    ui->registerFracBitsDisplay ->setText(fracBits              ? QString::number(*fracBits)    : QString(""));
    ui->registeSignBitDisplay   ->setText(signBit               ? QString::number(*signBit)     : QString(""));
}
