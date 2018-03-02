#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>

class PropertiesWidget : public QWidget {

public:
  PropertiesWidget(QWidget *parent);

  void clearFields();
  void setSize(int nOfElements, int size = 1);
  void setNames(std::vector<std::string> components);

  // Numerical addressed -only methods of the interface
  void setFixedPointInfo(int width, int fracBits, int signBit);
  void setAddress(int bar, int address);
};

#endif // PROPERTIESWIDGET_H
