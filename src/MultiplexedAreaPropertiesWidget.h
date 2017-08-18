#ifndef MULTIPLEXEDAREAPROPERTIESWIDGET_H
#define MULTIPLEXEDAREAPROPERTIESWIDGET_H

#include "CustomDelegates.h"
#include "PropertiesWidget.h"
#include "ui_MultiplexedAreaPropertiesWidget.h"

#include "NumericDataTableMixin.h"

namespace Ui {
class MultiplexedAreaPropertiesWidget;
}

class MultiplexedAreaPropertiesWidget : public PropertiesWidget {
  Q_OBJECT

public:
  explicit MultiplexedAreaPropertiesWidget(QWidget *parent);
  ~MultiplexedAreaPropertiesWidget();

  // PropertiesWidget interface implementation

  virtual void clearFields();
  virtual void setSize(int nOfElements, int nOfChannels);
  virtual void setNames(std::vector<std::string> components);
  virtual void setFixedPointInfo(int width = 0, int fracBits = 0,
                                 int signBit = 0);
  virtual void setAddress(int bar, int address);

public:
  Ui::MultiplexedAreaPropertiesWidget *ui;

  friend class QtHardMon;
  friend class PlotWindow;
};

#endif // MULTIPLEXEDAREAPROPERTIESWIDGET_H
