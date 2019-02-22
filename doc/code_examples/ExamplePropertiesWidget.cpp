#include "ExamplePropertiesWidget.h"
#include "Constants.h"

ExamplePropertiesWidget::ExamplePropertiesWidget(QWidget* parent)
: PropertiesWidget(parent), ui(new Ui::ExamplePropertiesWidget) {
  ui->setupUi(this);
  clearFields();

  connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(updateTable(int, int)));

  setTableWidget(ui->valuesTableWidget);
}

ExamplePropertiesWidget::~ExamplePropertiesWidget() {
  delete ui;
}

void ExamplePropertiesWidget::updateTable(int row, int column) {
  updateTableEntries(row, column);
}

void ExamplePropertiesWidget::clearFields() {
  ui->registerNameDisplay->setText("");
  ui->moduleDisplay->setText("");
  ui->registerNElementsDisplay->setText("");
}

void ExamplePropertiesWidget::setSize(int nOfElements, int /* size */) {
  ui->registerNElementsDisplay->setText(QString::number(nOfElements));
}

void ExamplePropertiesWidget::setNames(std::vector<std::string> components) {
  std::string moduleName;
  if(components.size() >= 2) {
    for(unsigned int i = 0; i < components.size() - 1; ++i) {
      moduleName += "/" + components[i];
    }
    moduleName = moduleName.substr(1);
  }
  else
    moduleName = "";

  ui->registerNameDisplay->setText(components.back().c_str());
  ui->moduleDisplay->setText(moduleName.c_str());
}

void ExamplePropertiesWidget::setFixedPointInfo(int /* width */, int /* fracBits */, int /* signBit */) {}

void ExamplePropertiesWidget::setAddress(int /* bar */, int /* address */) {}