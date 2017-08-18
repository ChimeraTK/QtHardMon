#include "GenericRegisterPropertiesWidget.h"
#include "Constants.h"

GenericRegisterPropertiesWidget::GenericRegisterPropertiesWidget(
    QWidget *parent)
    : PropertiesWidget(parent), ui(new Ui::GenericRegisterPropertiesWidget) {
  ui->setupUi(this);
  clearFields();

  connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)), this,
          SLOT(updateTable(int, int)));

  setTableWidget(ui->valuesTableWidget);
}

GenericRegisterPropertiesWidget::~GenericRegisterPropertiesWidget() {
  delete ui;
}

void GenericRegisterPropertiesWidget::updateTable(int row, int column) {
  updateTableEntries(row, column);
}

void GenericRegisterPropertiesWidget::clearFields() {
  ui->registerNameDisplay->setText("");
  ui->moduleDisplay->setText("");
  ui->registerNElementsDisplay->setText("");
}

void GenericRegisterPropertiesWidget::setSize(int nOfElements, int /* size */) {
  ui->registerNElementsDisplay->setText(QString::number(nOfElements));
}

void GenericRegisterPropertiesWidget::setNames(
    std::vector<std::string> components) {
  std::string moduleName;
  if (components.size() >= 2) {
    for (unsigned int i = 0; i < components.size() - 1; ++i) {
      moduleName += "/" + components[i];
    }
    moduleName = moduleName.substr(1);
  } else
    moduleName = "";

  ui->registerNameDisplay->setText(components.back().c_str());
  ui->moduleDisplay->setText(moduleName.c_str());
}

void GenericRegisterPropertiesWidget::setFixedPointInfo(int /* width */,
                                                        int /* fracBits */,
                                                        int /* signBit */) {}

void GenericRegisterPropertiesWidget::setAddress(int /* bar */,
                                                 int /* address */) {}