#include "NumericAddressedRegisterQTreeItem.h"
#include "RegisterTreeUtilities.h"

NumericAddressedRegisterQTreeItem::NumericAddressedRegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(QString(RegisterTreeUtilities::getRegisterName(registerInfo).c_str()),
  static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType), 
  RegisterTreeUtilities::assignToModuleItem(registerInfo, parent, propertiesWidget)),
  oneDRegisterAccessor_(device.getOneDRegisterAccessor<double>(registerInfo->getRegisterName())),
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

void NumericAddressedRegisterQTreeItem::readData() {
  oneDRegisterAccessor_.read();
  QTableWidget* table = propertiesWidget_->ui->valuesTableWidget;
  table->clearContents();
  table->setRowCount(0);
  table->setRowCount(oneDRegisterAccessor_.getNElements());
  for (unsigned int row = 0; row < oneDRegisterAccessor_.getNElements(); ++row) {
    QTableWidgetItem* dataItem = new QTableWidgetItem();
    QTableWidgetItem* rowItem = new QTableWidgetItem();

    std::stringstream rowAsText;
    rowAsText << row;
    rowItem->setText(rowAsText.str().c_str());
    table->setVerticalHeaderItem(row, rowItem);

    // if (row == maxRow) { // The register is too large to display. Show that it
    //                      // is truncated and stop reading
    //   dataItem->setText("truncated");
    //   dataItem->setFlags(dataItem->flags() & ~Qt::ItemIsSelectable &
    //                      ~Qt::ItemIsEditable);
    //   dataItem->setToolTip("List is truncated. You can change the number of "
    //                        "words displayed in the preferences.");
    //   table->setItem(row, 0, dataItem);
    //   break;
    // }

    dataItem->setData(0, QVariant(oneDRegisterAccessor_[row]));
    table->setItem(row, 2, dataItem);
  }
}

void NumericAddressedRegisterQTreeItem::writeData() {
     QTableWidget* table = propertiesWidget_->ui->valuesTableWidget;
     for (unsigned int row = 0; row < oneDRegisterAccessor_.getNElements(); ++row) {
      oneDRegisterAccessor_[row] = table->item(row, 2)->data(0).toDouble();
     }
     oneDRegisterAccessor_.write();
}

void NumericAddressedRegisterQTreeItem::updateRegisterProperties() {
  propertiesWidget_->setRegisterProperties(*properties_);
  propertiesWidget_->setFixedPointConverter(fixedPointConverter_);
}