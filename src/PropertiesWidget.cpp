#include "PropertiesWidget.h"

PropertiesWidget::PropertiesWidget(QWidget *parent) : QWidget(parent) {
  ui.setupUi(this);
}

void PropertiesWidget::clearFields(){
  ui.registerPathDisplay->setText("");
  ui.dimensionDisplay->setText("");
  ui.nElementsDisplay->setText("");
  ui.nChannelsDisplay->setText("");
  ui.numericalAddresseGroupBox->hide();
  ui.fixedPointGroupBox->hide();
}

void PropertiesWidget::updateRegisterInfo(boost::shared_ptr<mtca4u::RegisterInfo> const & registerInfo){
  ui.registerPathDisplay->setText(static_cast<const std::string &>(registerInfo->getRegisterName()).c_str());
  ui.dimensionDisplay->setText(QString::number(registerInfo->getNumberOfDimensions()));
  ui.nElementsDisplay->setText(QString::number(registerInfo->getNumberOfElements()));
  ui.nChannelsDisplay->setText(QString::number(registerInfo->getNumberOfChannels()));
  ///@todo FIXME fill numerial and fixed point information
  ui.numericalAddresseGroupBox->hide();
  ui.fixedPointGroupBox->hide();
}

void PropertiesWidget::clearDataWidgetBackground(){
}
