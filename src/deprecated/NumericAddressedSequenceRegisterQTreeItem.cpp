#include "NumericAddressedSequenceRegisterQTreeItem.h"

NumericAddressedSequenceRegisterQTreeItem::
    NumericAddressedSequenceRegisterQTreeItem(
        boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
        mtca4u::TwoDRegisterAccessor<double> &twoDRegisterAccessor,
        unsigned int channelNo, QTreeWidgetItem *parent,
        PropertiesWidgetProvider &propertiesWidgetProvider)
    : DeviceElementQTreeItem(
          QString(
              registerInfo->getRegisterName().getComponents().back().c_str()),
          static_cast<int>(DeviceElementDataType::SequenceRegisterDataType),
          parent, propertiesWidgetProvider),
      twoDRegisterAccessor_(twoDRegisterAccessor), channelNo_(channelNo) {
  mtca4u::RegisterInfoMap::RegisterInfo *numericAddressedRegisterInfo =
      dynamic_cast<mtca4u::RegisterInfoMap::RegisterInfo *>(registerInfo.get());

  name_ = registerInfo->getRegisterName().getComponents();

  if (numericAddressedRegisterInfo) {
    bar_ = numericAddressedRegisterInfo->bar;
    address_ = numericAddressedRegisterInfo->address;
    size_ = numericAddressedRegisterInfo->nBytes;
    width_ = numericAddressedRegisterInfo->width;
    fracBits_ = numericAddressedRegisterInfo->nFractionalBits;
    signFlag_ = numericAddressedRegisterInfo->signedFlag;

    fixedPointConverter_ = new mtca4u::FixedPointConverter(
        numericAddressedRegisterInfo->getRegisterName().getComponents().back(),
        numericAddressedRegisterInfo->width,
        numericAddressedRegisterInfo->nFractionalBits,
        numericAddressedRegisterInfo->signedFlag);

  } else {
    // FIXME: the cast was invalid, we have assigned wrong
    // DeviceElementQTreeItem.
  }
}
void NumericAddressedSequenceRegisterQTreeItem::readData(mtca4u::Device &device) {
  //todo ... build accessor on item selection
  twoDRegisterAccessor_.read();
  QTableWidget *table =
      dynamic_cast<RegisterPropertiesWidget *>(getPropertiesWidget())
          ->ui->valuesTableWidget;
  table->clearContents();
  table->setRowCount(0);
  unsigned int elementsPerChannel =
      twoDRegisterAccessor_.getNElementsPerChannel();
  table->setRowCount(elementsPerChannel);

  for (unsigned int row = 0; row < elementsPerChannel; ++row) {
    QTableWidgetItem *dataItem = new QTableWidgetItem();
    QTableWidgetItem *rowItem = new QTableWidgetItem();

    std::stringstream rowAsText;
    rowAsText << row;
    rowItem->setText(rowAsText.str().c_str());
    table->setVerticalHeaderItem(row, rowItem);

    dataItem->setData(0, QVariant(twoDRegisterAccessor_[channelNo_][row]));
    table->setItem(row, 2, dataItem);
  }
}

void NumericAddressedSequenceRegisterQTreeItem::writeData(mtca4u::Device &device) {
	//todo ... build accessor on item selection
  QTableWidget *table =
      dynamic_cast<RegisterPropertiesWidget *>(getPropertiesWidget())
          ->ui->valuesTableWidget;
  for (unsigned int row = 0;
       row < twoDRegisterAccessor_.getNElementsPerChannel(); ++row) {
    twoDRegisterAccessor_[channelNo_][row] =
        table->item(row, 2)->data(0).toDouble();
  }
  twoDRegisterAccessor_.write();
}

void NumericAddressedSequenceRegisterQTreeItem::updateRegisterProperties(mtca4u::Device &device) {
	//todo ... build accessor on item selection
  getPropertiesWidget()->clearFields();
  getPropertiesWidget()->setNames(name_);
  getPropertiesWidget()->setSize(twoDRegisterAccessor_.getNElementsPerChannel(),
                                 size_);
  getPropertiesWidget()->setAddress(bar_, address_);
  getPropertiesWidget()->setFixedPointInfo(width_, fracBits_, signFlag_);
  RegisterPropertiesWidget *registerPropertiesCast =
      dynamic_cast<RegisterPropertiesWidget *>(getPropertiesWidget());
  if (registerPropertiesCast) {
    registerPropertiesCast->setFixedPointConverter(fixedPointConverter_);
  }
}
