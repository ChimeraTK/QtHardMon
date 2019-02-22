#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <boost/shared_ptr.hpp>

#include "ui_PropertiesWidgetForm.h"
#include <ChimeraTK/RegisterInfo.h>
#include <QWidget>

class PropertiesWidget : public QWidget {
  //  Q_OBJECT

 public:
  PropertiesWidget(QWidget* parent);

  void clearFields();

  Ui::PropertiesWidgetForm ui;

  void updateRegisterInfo(boost::shared_ptr<ChimeraTK::RegisterInfo> const& registerInfo);

  /// Clear the red cells of the data display widget after writing
  void clearDataWidgetBackground();

 protected:
  // Numerical addressed -only methods of the interface
  void setFixedPointInfo(int width, int nFracBits, bool signedFlag, unsigned int nDimensions);
  void setAddress(int bar, int address, int sizeInBytes);
  // other internal methods to avoid screens of unstructured code...
  void setShape(unsigned int nDimensions, unsigned int nChannels, unsigned int nElements);
  void setType(ChimeraTK::RegisterInfo::DataDescriptor const& dataDescriptor);

  void setOneDWidgetsVisible(bool visible);
  void setTwoDWidgetsVisible(bool visible);
};

#endif // PROPERTIESWIDGET_H
