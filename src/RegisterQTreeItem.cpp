#include "RegisterQTreeItem.h"
#include "RegisterTreeUtilities.h"

RegisterQTreeItem::RegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, PropertiesWidgetProvider & propertiesWidgetProvider)
: DeviceElementQTreeItem(QString(RegisterTreeUtilities::getRegisterName(registerInfo).c_str()),
  static_cast<int>(DeviceElementDataType::GenericRegisterDataType), 
  RegisterTreeUtilities::assignToModuleItem(registerInfo, parent, propertiesWidgetProvider), propertiesWidgetProvider),
  oneDRegisterAccessor_(device.getOneDRegisterAccessor<double>(registerInfo->getRegisterName()))
{
    QString registerName = QString(registerInfo->getRegisterName().getComponents().back().c_str());
    QString moduleName = QString(registerInfo->getRegisterName().getComponents().front().c_str());
    QString bar = QString("");
    QString nOfElements = QString::number(registerInfo->getNumberOfElements());
    QString address = QString("");
    QString size = QString("");
    QString width = QString("");
    QString fracBits = QString("");
    QString signFlag = QString("");

    properties_ = new RegisterPropertiesWidget::RegisterProperties(registerName, 
    moduleName, bar, address, nOfElements, size, width, fracBits, signFlag);
}

void RegisterQTreeItem::readData() {
  oneDRegisterAccessor_.read();
  QTableWidget* table = dynamic_cast<RegisterPropertiesWidget*>(getPropertiesWidget())->ui->valuesTableWidget;
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

void RegisterQTreeItem::writeData() {
     QTableWidget* table = dynamic_cast<RegisterPropertiesWidget*>(getPropertiesWidget())->ui->valuesTableWidget;
     for (unsigned int row = 0; row < oneDRegisterAccessor_.getNElements(); ++row) {
      oneDRegisterAccessor_[row] = table->item(row, 2)->data(0).toDouble();
     }
     oneDRegisterAccessor_.write();
}

void RegisterQTreeItem::updateRegisterProperties() {
  dynamic_cast<RegisterPropertiesWidget*>(getPropertiesWidget())->setRegisterProperties(*properties_);
  dynamic_cast<RegisterPropertiesWidget*>(getPropertiesWidget())->setFixedPointConverter(nullptr);
}