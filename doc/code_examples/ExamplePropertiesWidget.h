#ifndef EXAMPLEPROPERTIESWIDGET_H
#define EXAMPLEPROPERTIESWIDGET_H

#include "NumericDataTableMixin.h"
#include "PropertiesWidget.h"
#include "ui_ExamplePropertiesWidget.h"

namespace Ui {
  class ExamplePropertiesWidget;
}

class ExamplePropertiesWidget : public PropertiesWidget, public NumericDataTableMixin {
  Q_OBJECT

 public:
  explicit ExamplePropertiesWidget(QWidget* parent);
  ~ExamplePropertiesWidget();

  virtual void clearFields();
  virtual void setSize(int nOfElements, int size = 0);
  virtual void setNames(std::vector<std::string> components);
  virtual void setFixedPointInfo(int width = 0, int fracBits = 0, int signBit) = 0;
  virtual void setAddress(int bar = 0, int address = 0);

 private slots:
  void updateTable(int row, int column);

 public:
  Ui::ExamplePropertiesWidget* ui;
};

#endif // EXAMPLEPROPERTIESWIDGET_H