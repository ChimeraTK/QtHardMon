#include "ExampleDeviceElementQTreeItem.h"
#include "ExamplePropertiesWidget.h"
#include "RegisterTreeUtilities.h"

ExampleDeviceElementQTreeItem::ExampleDeviceElementQTreeItem(
    mtca4u::Device &device,
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget *parent,
    PropertiesWidgetProvider &propertiesWidgetProvider)
    : DeviceElementQTreeItem(
          QString(RegisterTreeUtilities::getRegisterName(registerInfo).c_str()),
          static_cast<int>(/* DataType*/),
          RegisterTreeUtilities::assignToModuleItem(registerInfo, parent,
                                                    propertiesWidgetProvider),
          propertiesWidgetProvider),
      oneDRegisterAccessor_(device.getOneDRegisterAccessor<double>(
          registerInfo->getRegisterName())) {
  name_ = registerInfo->getRegisterName().getComponents();
}

void ExampleDeviceElementQTreeItem::readData() {

  // Read...

  QTableWidget *table =
      dynamic_cast<NumericDataTableMixin *>(getPropertiesWidget())
          ->getTableWidget();
  if (table) {
    table->clearContents();
    table->setRowCount(0);
    table->setRowCount(/* nOfElements */);
    for (unsigned int row = 0; row < /* nOfElements */; ++row) {
      QTableWidgetItem *dataItem = new QTableWidgetItem();
      QTableWidgetItem *rowItem = new QTableWidgetItem();

      std::stringstream rowAsText;
      rowAsText << row;
      rowItem->setText(rowAsText.str().c_str());
      table->setVerticalHeaderItem(row, rowItem);

      dataItem->setData(0, QVariant(/* data */));
      table->setItem(row, 2, dataItem);
    }
  }
}

void ExampleDeviceElementQTreeItem::writeData() {
  QTableWidget *table =
      dynamic_cast<NumericDataTableMixin *>(getPropertiesWidget())
          ->getTableWidget();
  if (table) {
    for (unsigned int row = 0; row < /* nOfElements */; ++row) {
      /* data */ = table->item(row, 2)->data(0).toDouble();
    }

    // Write...
  }
}

void ExampleDeviceElementQTreeItem::updateRegisterProperties() {
  getPropertiesWidget()->clearFields();
  getPropertiesWidget()->setNames(name_);
  getPropertiesWidget()->setSize(/* nOfElements */);
}