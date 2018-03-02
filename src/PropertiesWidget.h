#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>
#include "ui_PropertiesWidgetForm.h"
#include <mtca4u/RegisterInfo.h>

class PropertiesWidget : public QWidget {
//  Q_OBJECT

public:
  PropertiesWidget(QWidget *parent);

  void clearFields();
  void setSize(int nOfElements, int size = 1);
  void setNames(std::vector<std::string> components);

  // Numerical addressed -only methods of the interface
  void setFixedPointInfo(int width, int fracBits, int signBit);
  void setAddress(int bar, int address);

  Ui::PropertiesWidgetForm ui;

  void updateRegisterInfo(boost::shared_ptr<mtca4u::RegisterInfo> const & registerInfo);

  /// Clear the red cells of the data display widget after writing
  void clearDataWidgetBackground();
};

#endif // PROPERTIESWIDGET_H
