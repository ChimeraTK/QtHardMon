#include "NumericAddressedRegisterQTreeItem.h"
#include "Exceptions.h"
#include "ModuleQTreeItem.h"

// FIXME: the assignment of text is done on initializer list by using dynamic_cast - this might throw.
// Move to constructor body and throw grafefully.
NumericAddressedRegisterQTreeItem::NumericAddressedRegisterQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(QString(getRegisterName(registerInfo).c_str()),
  static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType), 
  assignToModuleItem(registerInfo, parent)),
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
     oneDRegisterAccessor_.write();
}

void NumericAddressedRegisterQTreeItem::updateRegisterProperties() {
  propertiesWidget_->setRegisterProperties(*properties_);
  propertiesWidget_->setFixedPointConverter(fixedPointConverter_);
}

QTreeWidgetItem * NumericAddressedRegisterQTreeItem::assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * treeWidget) {
  
    std::vector<std::string> registerPathComponents = registerInfo->getRegisterName().getComponents();
    std::string moduleName = registerPathComponents.front();
    
    // FIXME: There is a high chance, that there are always two components.
    if (registerPathComponents.size() < 2 || moduleName.empty()) {
      moduleName = NO_MODULE_NAME_STRING;
    }

    QList<QTreeWidgetItem *> moduleList = 
      treeWidget->findItems(QString(moduleName.c_str()), Qt::MatchExactly);

    QTreeWidgetItem * moduleItem;
    if (moduleList.empty()) {
      moduleItem = new ModuleQTreeItem(QString(moduleName.c_str()), treeWidget, propertiesWidget_);
    } else {
      moduleItem = moduleList.front();
    }

    return moduleItem;
}

std::string NumericAddressedRegisterQTreeItem::getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  return registerInfo->getRegisterName().getComponents().back();
}