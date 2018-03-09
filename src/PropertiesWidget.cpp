#include "PropertiesWidget.h"
#include <ChimeraTK/RegisterInfoMap.h>

PropertiesWidget::PropertiesWidget(QWidget *parent) : QWidget(parent) {
  ui.setupUi(this);
  clearFields();// also hides what should be hidden
}

void PropertiesWidget::clearFields(){
  ui.registerPathDisplay->setText("");
  ui.dimensionDisplay->setText("");
  // no need to update channels or elements. They are hidden as info of 1D and 2D
  ui.numericalAddresseGroupBox->hide();
  ui.fixedPointGroupBox->hide();
  setOneDWidgetsVisible(false);
  setTwoDWidgetsVisible(false);
  setEnabled(false);
}

void PropertiesWidget::setOneDWidgetsVisible(bool visible){
  ui.nElementsLabel->setVisible(visible);
  ui.nElementsDisplay->setVisible(visible);
}

void PropertiesWidget::setTwoDWidgetsVisible(bool visible){
  ui.nChannelsLabel->setVisible(visible);
  ui.nChannelsDisplay->setVisible(visible);
  ui.channelWidget->setVisible(visible);
}

void PropertiesWidget::updateRegisterInfo(boost::shared_ptr<mtca4u::RegisterInfo> const & registerInfo){
  // There are node elements ("modules") which don't have register information
  if (!registerInfo){
    clearFields();
    return;
  }

  setEnabled(true);
  ui.registerPathDisplay->setText(static_cast<const std::string &>(registerInfo->getRegisterName()).c_str());
  setShape(registerInfo->getNumberOfDimensions(), registerInfo->getNumberOfChannels(), registerInfo->getNumberOfElements());
  setType(registerInfo->getDataDescriptor());

  // Try to cast to the old, numeric addressed registerInfo.
  auto numericAddressedInfo = boost::dynamic_pointer_cast<ChimeraTK::RegisterInfoMap::RegisterInfo>(registerInfo);
  // If the cast succeeded fill the numeric addressed and fixed point fields. This info only
  // exists for this type of registers.
  if(numericAddressedInfo){
    setAddress(numericAddressedInfo->bar, numericAddressedInfo->address, numericAddressedInfo->nBytes);
    setFixedPointInfo(numericAddressedInfo->width, numericAddressedInfo->nFractionalBits, numericAddressedInfo->signedFlag, registerInfo->getNumberOfDimensions());
  }else{ 
    ui.numericalAddresseGroupBox->hide();
    ui.fixedPointGroupBox->hide();
  }
}

void PropertiesWidget::setShape(unsigned int nDimensions, unsigned int nChannels, unsigned int nElements){
  switch (nDimensions){
  case 0:
    ui.dimensionDisplay->setText("Scalar");
    setOneDWidgetsVisible(false);
    setTwoDWidgetsVisible(false);
    ui.channelSpinBox->setValue(0);
    break;
  case 1:
    ui.dimensionDisplay->setText("1 D");
    setOneDWidgetsVisible(true);
    ui.nElementsDisplay->setText(QString::number(nElements));
    setTwoDWidgetsVisible(false);
    ui.channelSpinBox->setValue(0);
    break;
  case 2:
    ui.dimensionDisplay->setText("2 D");
    setOneDWidgetsVisible(true);
    ui.nElementsDisplay->setText(QString::number(nElements));
    setTwoDWidgetsVisible(true);
    ui.nChannelsDisplay->setText(QString::number(nChannels));
    ui.channelSpinBox->setMaximum(nChannels>0?nChannels-1:0);
    break;
  default:
    // we should never run into this, but who hows how device access will develop
    ui.dimensionDisplay->setText(QString::number(nDimensions));
    setOneDWidgetsVisible(true);
    ui.nElementsDisplay->setText(QString::number(nElements));
    setTwoDWidgetsVisible(true);
    ui.nChannelsDisplay->setText(QString::number(nChannels));
    ui.channelSpinBox->setMaximum(nChannels>0?nChannels-1:0);
  }
}

void PropertiesWidget::setType(ChimeraTK::RegisterInfo::DataDescriptor const & dataDescriptor){
  switch(dataDescriptor.fundamentalType()){
  case ChimeraTK::RegisterInfo::FundamentalType::numeric: 	
    if (dataDescriptor.isIntegral()){
      if (dataDescriptor.isSigned()){
        ui.dataTypeDisplay->setText("Signed integer");
      }else{
        ui.dataTypeDisplay->setText("Unsingned integer");
      }
    }else{
      ui.dataTypeDisplay->setText("Floating point");
    }
    break;
  case ChimeraTK::RegisterInfo::FundamentalType::string:
    ui.dataTypeDisplay->setText("String");
    break;
  case ChimeraTK::RegisterInfo::FundamentalType::boolean:
    ui.dataTypeDisplay->setText("Boolean");
    break;
  case ChimeraTK::RegisterInfo::FundamentalType::nodata:
    ui.dataTypeDisplay->setText("No Data");
    break;
  case ChimeraTK::RegisterInfo::FundamentalType::undefined:
    // fall into default, just mentioned for completeness
  default:
    ui.dataTypeDisplay->setText("Undefined");
  }
}

void PropertiesWidget::setAddress(int bar, int address, int sizeInBytes){
  ui.numericalAddresseGroupBox->show();
  ui.barDisplay->setText(QString::number(bar));
  ui.addressDisplay->setText(QString::number(address));
  ui.sizeDisplay->setText(QString::number(sizeInBytes));
}

void PropertiesWidget::setFixedPointInfo(int width, int nFracBits, bool signedFlag, unsigned int nDimensions){
  // Fixed point information is only valid for scalar and 1D registers.
  // In 2D registers it is per channel.
  if (nDimensions >1){
    ui.fixedPointGroupBox->hide();
    return;
  }

  ui.fixedPointGroupBox->show();   
  ui.widthDisplay->setText(QString::number(width));
  ui.nFracBitsDisplay->setText(QString::number(nFracBits));
  ui.signedFlagDisplay->setText(signedFlag?"1":"0");
}

void PropertiesWidget::clearDataWidgetBackground(){
}
