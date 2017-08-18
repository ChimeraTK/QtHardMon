#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>

class PropertiesWidget : public QWidget {

public:
  PropertiesWidget(QWidget *parent);

  virtual void clearFields() = 0;
  virtual void setSize(int nOfElements, int size = 1) = 0;
  virtual void setNames(std::vector<std::string> components) = 0;

  // Numerical addressed -only methods of the interface
  virtual void setFixedPointInfo(int width, int fracBits, int signBit) = 0;
  virtual void setAddress(int bar, int address) = 0;
};

#endif // PROPERTIESWIDGET_H