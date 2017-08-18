#ifndef GENERICREGISTERPROPERTIESWIDGET_H
#define GENERICREGISTERPROPERTIESWIDGET_H

#include "CustomDelegates.h"
#include "NumericDataTableMixin.h"
#include "PropertiesWidget.h"
#include "ui_GenericRegisterPropertiesWidget.h"

namespace Ui {
class GenericRegisterPropertiesWidget;
}

class GenericRegisterPropertiesWidget : public PropertiesWidget,
                                        public NumericDataTableMixin {
  Q_OBJECT

public:
public:
  explicit GenericRegisterPropertiesWidget(QWidget *parent);
  ~GenericRegisterPropertiesWidget();

  virtual void clearFields();
  virtual void setSize(int nOfElements, int nOfChannels);
  virtual void setNames(std::vector<std::string> components);
  virtual void setFixedPointInfo(int width, int fracBits, int signBit);
  virtual void setAddress(int bar, int address);

private slots:
  void updateTable(int row, int column);

public:
  Ui::GenericRegisterPropertiesWidget *ui;

  friend class QtHardMon;
  friend class PlotWindow;
};

#endif // GENERICREGISTERPROPERTIESWIDGET_H