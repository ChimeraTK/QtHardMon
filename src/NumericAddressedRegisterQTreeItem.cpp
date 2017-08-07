#include "NumericAddressedRegisterQTreeItem.h"
#include "Exceptions.h"
#include "ModuleQTreeItem.h"

NumericAddressedRegisterQTreeItem::NumericAddressedRegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget)
: NumericAddressedElementQTreeItem(device, registerInfo, parent, propertiesWidget),
  oneDRegisterAccessor_(device.getOneDRegisterAccessor<double>(registerInfo->getRegisterName()))
{
}

void NumericAddressedRegisterQTreeItem::read() {
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

    dataItem->setData(0, QVariant(oneDRegisterAccessor_[row]));
    table->setItem(row, 2, dataItem);
  }
}

void NumericAddressedRegisterQTreeItem::write() {
     QTableWidget* table = propertiesWidget_->ui->valuesTableWidget;
     for (unsigned int row = 0; row < oneDRegisterAccessor_.getNElements(); ++row) {
      oneDRegisterAccessor_[row] = table->item(row, 2)->data(0).toDouble();
     }
     oneDRegisterAccessor_.write();
}