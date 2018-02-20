#include "NumericAddressedRegisterQTreeItem.h"
#include "RegisterTreeUtilities.h"

NumericAddressedRegisterQTreeItem::NumericAddressedRegisterQTreeItem(
    mtca4u::Device &device,
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget *parent,
    PropertiesWidgetProvider &propertiesWidgetProvider)
    : DeviceElementQTreeItem(
          QString(RegisterTreeUtilities::getRegisterName(registerInfo).c_str()),
          static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType),
          RegisterTreeUtilities::assignToModuleItem(registerInfo, parent,
                                                    propertiesWidgetProvider),
          propertiesWidgetProvider),oneDRegisterAccessor_(){
	registerInfo_ = registerInfo;
	name_ = registerInfo->getRegisterName().getComponents();
}
void NumericAddressedRegisterQTreeItem::getRegisterInfo(mtca4u::Device &device)
{
	oneDRegisterAccessor_.replace(device.getOneDRegisterAccessor<double>(registerInfo_->getRegisterName()));
	  mtca4u::RegisterInfoMap::RegisterInfo *numericAddressedRegisterInfo =
	      dynamic_cast<mtca4u::RegisterInfoMap::RegisterInfo *>(registerInfo_.get());

	  name_ = registerInfo_->getRegisterName().getComponents();

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

void NumericAddressedRegisterQTreeItem::readData(mtca4u::Device &device)
{
  //to do .. load accessor only if item changes.
  getRegisterInfo(device);
  oneDRegisterAccessor_.read();
  QTableWidget *table =
      dynamic_cast<RegisterPropertiesWidget *>(getPropertiesWidget())
          ->ui->valuesTableWidget;
  table->clearContents();
  table->setRowCount(0);
  table->setRowCount(oneDRegisterAccessor_.getNElements());
  //getPropertiesWidget()->_cellChanged = false;
  for (unsigned int row = 0; row < oneDRegisterAccessor_.getNElements();
       ++row) {
    QTableWidgetItem *dataItem = new QTableWidgetItem();
    QTableWidgetItem *rowItem = new QTableWidgetItem();

    std::stringstream rowAsText;
    rowAsText << row;
    rowItem->setText(rowAsText.str().c_str());
    table->setVerticalHeaderItem(row, rowItem);

    // if (row == maxRow) { // The register is too large to display. Show that
    // it
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

void NumericAddressedRegisterQTreeItem::writeData(mtca4u::Device &device)
{
	//getRegisterInfo(device); //read did it already
	QTableWidget *table =
      dynamic_cast<RegisterPropertiesWidget *>(getPropertiesWidget())
          ->ui->valuesTableWidget;
  for (unsigned int row = 0; row < oneDRegisterAccessor_.getNElements();
       ++row) {
    oneDRegisterAccessor_[row] = table->item(row, 2)->data(0).toDouble();
  }
  oneDRegisterAccessor_.write();
}

void NumericAddressedRegisterQTreeItem::updateRegisterProperties(mtca4u::Device &device) {
  getRegisterInfo(device);
  getPropertiesWidget()->clearFields();
  getPropertiesWidget()->setNames(name_);
  getPropertiesWidget()->setSize(oneDRegisterAccessor_.getNElements(), size_);
  getPropertiesWidget()->setAddress(bar_, address_);
  getPropertiesWidget()->setFixedPointInfo(width_, fracBits_, signFlag_);
  dynamic_cast<NumericDataTableMixin *>(getPropertiesWidget())
      ->setFixedPointConverter(fixedPointConverter_);
}
