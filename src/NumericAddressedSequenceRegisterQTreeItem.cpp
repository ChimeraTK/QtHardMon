#include "NumericAddressedSequenceRegisterQTreeItem.h"

NumericAddressedSequenceRegisterQTreeItem::NumericAddressedSequenceRegisterQTreeItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, mtca4u::TwoDRegisterAccessor<double> & twoDRegisterAccessor, unsigned int channelNo, QTreeWidgetItem * parent, RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(QString(registerInfo->getRegisterName().getComponents().back().c_str()),
static_cast<int>(DeviceElementDataType::SequenceRegisterDataType), parent),
  twoDRegisterAccessor_(twoDRegisterAccessor),
  channelNo_(channelNo),
  propertiesWidget_(propertiesWidget)
{
  mtca4u::RegisterInfoMap::RegisterInfo * numericAddressedRegisterInfo = dynamic_cast<mtca4u::RegisterInfoMap::RegisterInfo *>(registerInfo.get());

  if (numericAddressedRegisterInfo) {
    QString registerName = QString(numericAddressedRegisterInfo->getRegisterName().getComponents().back().c_str());
    QString moduleName = QString(numericAddressedRegisterInfo->getRegisterName().getComponents().front().c_str());
    QString bar = QString::number(numericAddressedRegisterInfo->bar);
    QString nOfElements = QString::number(numericAddressedRegisterInfo->getNumberOfElements());
    QString address = QString::number(numericAddressedRegisterInfo->address);
    QString size = QString::number(numericAddressedRegisterInfo->nBytes);
    QString width = QString::number(numericAddressedRegisterInfo->width);
    QString fracBits = QString::number(numericAddressedRegisterInfo->nFractionalBits);
    QString signFlag = QString::number(numericAddressedRegisterInfo->signedFlag);

    properties_ = new RegisterPropertiesWidget::RegisterProperties(registerName, 
    moduleName, bar, address, nOfElements, size, width, fracBits, signFlag);

    fixedPointConverter_ = new mtca4u::FixedPointConverter(numericAddressedRegisterInfo->getRegisterName().getComponents().back(), numericAddressedRegisterInfo->width, numericAddressedRegisterInfo->nFractionalBits, numericAddressedRegisterInfo->signedFlag);

  } else {
    // FIXME: the cast was invalid, we have assigned wrong DeviceElementQTreeItem.
  }
}

void NumericAddressedSequenceRegisterQTreeItem::read() {
  twoDRegisterAccessor_.read();
  QTableWidget* table = propertiesWidget_->ui->valuesTableWidget;
  table->clearContents();
  table->setRowCount(0);
  unsigned int elementsPerChannel = twoDRegisterAccessor_.getNElementsPerChannel();
  table->setRowCount(elementsPerChannel);
  

  for(unsigned int row = 0; row < elementsPerChannel; ++row) {
    QTableWidgetItem* dataItem = new QTableWidgetItem();
    QTableWidgetItem* rowItem = new QTableWidgetItem();

    std::stringstream rowAsText;
    rowAsText << row;
    rowItem->setText(rowAsText.str().c_str());
    table->setVerticalHeaderItem(row, rowItem);

    dataItem->setData(0, QVariant(twoDRegisterAccessor_[channelNo_][row]));
    table->setItem(row, 2, dataItem);
  }
}

void NumericAddressedSequenceRegisterQTreeItem::write() {
     QTableWidget* table = propertiesWidget_->ui->valuesTableWidget;
     for (unsigned int row = 0; row < twoDRegisterAccessor_.getNElementsPerChannel(); ++row) {
      twoDRegisterAccessor_[channelNo_][row] = table->item(row, 2)->data(0).toDouble();
     }
     twoDRegisterAccessor_.write();
}

void NumericAddressedSequenceRegisterQTreeItem::updateRegisterProperties() {
  propertiesWidget_->setRegisterProperties(*properties_);
  propertiesWidget_->setFixedPointConverter(fixedPointConverter_);
}