#include "SequencePropertiesWidget.h"
#include "Constants.h"

SequencePropertiesWidget::SequencePropertiesWidget(QWidget *parent)
    : PropertiesWidget(parent), ui(new Ui::SequencePropertiesWidget) {
  ui->setupUi(this);
  clearFields();

  connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)), this,
          SLOT(updateTable(int, int)));

  setTableWidget(ui->valuesTableWidget);
}

void SequencePropertiesWidget::updateTable(int row, int column) {
  updateTableEntries(row, column);
}

SequencePropertiesWidget::~SequencePropertiesWidget() { delete ui; }

void SequencePropertiesWidget::clearFields() {
  RegisterProperties properties;
  setRegisterProperties(properties);
}

void SequencePropertiesWidget::setRegisterProperties(
    RegisterProperties properties) {
  ui->registerNameDisplay->setText(properties.registerName);
  ui->moduleDisplay->setText(properties.moduleName);
  ui->registerBarDisplay->setText(properties.bar);
  ui->registerNElementsDisplay->setText(properties.nOfElements);
  ui->registerAddressDisplay->setText(properties.address);
  ui->registerSizeDisplay->setText(properties.size);
  ui->registerWidthDisplay->setText(properties.width);
  ui->registerFracBitsDisplay->setText(properties.fracBits);
  ui->registeSignBitDisplay->setText(properties.signBit);
}

SequencePropertiesWidget::RegisterProperties::RegisterProperties(
    QString RegisterName, QString ModuleName, QString Bar, QString Address,
    QString NOfElements, QString Size, QString Width, QString FracBits,
    QString SignBit)
    : registerName(RegisterName), moduleName(ModuleName), bar(Bar),
      address(Address), nOfElements(NOfElements), size(Size), width(Width),
      fracBits(FracBits), signBit(SignBit) {}

void SequencePropertiesWidget::setSize(int nOfElements, int size) {
  ui->registerNElementsDisplay->setText(QString::number(nOfElements));
  ui->registerSizeDisplay->setText(QString::number(size));
}

void SequencePropertiesWidget::setNames(std::vector<std::string> components) {
  if (components.size() < 2) {
    // FIXME: throw - we do not have enough components but we should have.
    // Not throwing at the moment as SequencePropertiesWidget is a fallback
    // PropertiesWidget.
    ui->moduleDisplay->setText(components.front().c_str());
    ui->registerNameDisplay->setText("");
  } else {
    ui->registerNameDisplay->setText(components.back().c_str());
    ui->moduleDisplay->setText(components.front().c_str());
  }
}

void SequencePropertiesWidget::setFixedPointInfo(int width, int fracBits,
                                                 int signBit) {
  ui->registerWidthDisplay->setText(QString::number(width));
  ui->registerFracBitsDisplay->setText(QString::number(fracBits));
  ui->registeSignBitDisplay->setText(QString::number(signBit));
}

void SequencePropertiesWidget::setAddress(int bar, int address) {
  ui->registerBarDisplay->setText(QString::number(bar));
  ui->registerAddressDisplay->setText(QString::number(address));
}