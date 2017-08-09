#include "NumericAddressedCookedSequenceRegisterQTreeItem.h"

NumericAddressedCookedSequenceRegisterQTreeItem::NumericAddressedCookedSequenceRegisterQTreeItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, mtca4u::TwoDRegisterAccessor<double> & twoDRegisterAccessor, unsigned int channelNo, QTreeWidgetItem * parent, RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(QString((std::string("SEQUENCE_") + std::to_string(channelNo)).c_str()),
static_cast<int>(DeviceElementDataType::SequenceRegisterDataType), parent),
  twoDRegisterAccessor_(twoDRegisterAccessor),
  channelNo_(channelNo),
  propertiesWidget_(propertiesWidget)
{
}

void NumericAddressedCookedSequenceRegisterQTreeItem::read() {
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

void NumericAddressedCookedSequenceRegisterQTreeItem::write() {
     QTableWidget* table = propertiesWidget_->ui->valuesTableWidget;
     for (unsigned int row = 0; row < twoDRegisterAccessor_.getNElementsPerChannel(); ++row) {
      twoDRegisterAccessor_[channelNo_][row] = table->item(row, 2)->data(0).toDouble();
     }
     twoDRegisterAccessor_.write();
}

void NumericAddressedCookedSequenceRegisterQTreeItem::updateRegisterProperties() {
  dynamic_cast<DeviceElementQTreeItem*>(parent())->updateRegisterProperties();
}