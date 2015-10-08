/*
 * customQTreeItemInterface.cpp
 *
 *  Created on: Jul 14, 2015
 */

#include "CustomQTreeItem.h"
#include <boost/shared_ptr.hpp>
#include <mtca4u/DeviceBackend.h>
#include "Exceptions.h"

typedef boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > MuxedData_t;
typedef boost::shared_ptr<mtca4u::DeviceBackend> Device_t;
typedef mtca4u::RegisterInfoMap::RegisterInfo RegisterInfo_t;

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidget* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidgetItem* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

RegisterInfo_t const CustomQTreeItem::getRegisterMapElement() {
  return (RegisterInfo_t());
}

CustomQTreeItem::~CustomQTreeItem() {
  // TODO Auto-generated destructor stub
}
void CustomQTreeItem::fillTableWithDummyValues(
    const TableWidgetData& tableData) {
  RegisterInfo_t regInfo = this->getRegisterMapElement();
  unsigned int numElements = regInfo.reg_elem_nr;
  createTableRowEntries(tableData, numElements);
  std::vector<int> buffer(numElements, -1);
	putValuesIntoTable<int>(tableData, buffer);
}

void CustomQTreeItem::createTableRowEntries(const TableWidgetData& tabledata, unsigned int rows) {

	QTableWidget* table = tabledata.table;
  unsigned int maxRow = tabledata.tableMaxRowCount;

  int nRows = ( rows >  maxRow ?
  		maxRow + 1 :  rows );

  table->setRowCount(nRows);

  // set the
  for (int row = 0; row < nRows; ++row) {
    std::stringstream rowAsText;
    rowAsText << row;
    QTableWidgetItem* tableWidgetItem = new QTableWidgetItem();
    tableWidgetItem->setText(rowAsText.str().c_str());
    table->setVerticalHeaderItem(row, tableWidgetItem);
  }
}

ModuleItem::ModuleItem(const QString& text_, QTreeWidget* parent_)
    : CustomQTreeItem(text_, ModuleItem::DataType, parent_) {}

void ModuleItem::read(TableWidgetData const& tabledata) {
  createTableRowEntries(tabledata, 0);
  throw InvalidOperationException( "You cannot read from a module. Select a register.");
}

void ModuleItem::write(TableWidgetData const& /*tabledata*/) {
  throw InvalidOperationException( "You cannot write from a module. Select a register.");
}

void ModuleItem::updateRegisterProperties(const RegisterPropertyGrpBox& grpBox) {
	clearGrpBox(grpBox);
}

RegisterItem::RegisterItem(const RegisterInfo_t& registerInfo,
                           const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, RegisterItem::DataType, parent_),
      _registerMapElement(registerInfo) {}

void RegisterItem::read(TableWidgetData const& tabledata) {
  try {

  		std::vector<int> inputBuffer = fetchElementsFromCard(tabledata);
  		createTableRowEntries(tabledata, inputBuffer.size());
  		putValuesIntoTable(tabledata, inputBuffer);
  }
  catch (...) {
    fillTableWithDummyValues(tabledata);
    throw;
  }
}

std::vector<int> RegisterItem::fetchElementsFromCard(
    const TableWidgetData& tabledata) {
  RegisterInfo_t regInfo = this->getRegisterMapElement();

  int numberOfElements = regInfo.reg_elem_nr;
  std::vector<int> buffer(numberOfElements);

  unsigned int registerBar = regInfo.reg_bar;
  Device_t const& mtcadevice = tabledata.device;
  int maxrows = tabledata.tableMaxRowCount;
  size_t nBytesToRead =
      std::min(numberOfElements, maxrows) * qthardmon::WORD_SIZE_IN_BYTES;
  unsigned int registerAddress = regInfo.reg_address;

  if (registerBar == 0xD) {
    mtcadevice->readDMA(registerBar, registerAddress, &(buffer[0]),
    										nBytesToRead);
  } else {
    //mtcadevice->readArea(registerAddress, &(buffer[0]), nBytesToRead, registerBar);
  	mtcadevice->read(registerBar, registerAddress, &(buffer[0]), nBytesToRead);
  }
  return buffer;
}

void RegisterItem::write(TableWidgetData const& tabledata) {
	unsigned int numElementsinRegister = _registerMapElement.reg_elem_nr;
	std::vector<int> buffer = copyValuesFromTable<int>(tabledata, numElementsinRegister);
	writeRegisterToDevice(tabledata, buffer);
}

void RegisterItem::writeRegisterToDevice(TableWidgetData const& tabledata,
                                         const std::vector<int>& buffer) {
  Device_t const& mtcadevice = tabledata.device;
  unsigned int regAddress = _registerMapElement.reg_address;
  unsigned int regSizeinBytes = _registerMapElement.reg_size;
  unsigned int register bar = _registerMapElement.reg_bar;
  mtcadevice->write(regSizeinBytes, regAddress, &buffer[0], bar);
}

void RegisterItem::updateRegisterProperties(
    const RegisterPropertyGrpBox& grpBox) {
	fillGrpBox(grpBox, _registerMapElement);
}

const RegisterInfo_t RegisterItem::getRegisterMapElement() {
  return _registerMapElement;
}

MultiplexedAreaItem::MultiplexedAreaItem(
    boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const& accessor,
    const RegisterInfo_t& registerInfo, const QString& text_,
    QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, MultiplexedAreaItem::DataType, parent_),
      _dataAccessor(accessor),
      _registerMapElement(registerInfo) {}

void MultiplexedAreaItem::read(TableWidgetData const& tabledata) {
	// Nothing to read
	createTableRowEntries(tabledata, 0);
}

void MultiplexedAreaItem::write(TableWidgetData const& /*tabledata*/) {
  // do nothing? -> change behavior later to something better?
}

void MultiplexedAreaItem::updateRegisterProperties(
    const RegisterPropertyGrpBox& grpBox) {
  grpBox.registerNameDisplay->setText(_registerMapElement.reg_name.c_str());
  grpBox.moduleDisplay->setText(_registerMapElement.reg_module.c_str());
  grpBox.registerBarDisplay->setText(QString::number(_registerMapElement.reg_bar));
  grpBox.registerNElementsDisplay->setText("");
  grpBox.registerAddressDisplay->setText(QString::number(_registerMapElement.reg_address));
  grpBox.registerSizeDisplay->setText(QString::number(_registerMapElement.reg_size));

  grpBox.registerWidthDisplay->setText("");
  grpBox.registerFracBitsDisplay->setText("");
  grpBox.registeSignBitDisplay->setText("");
}


const RegisterInfo_t MultiplexedAreaItem::getRegisterMapElement() {
  return (_registerMapElement);
}
boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
MultiplexedAreaItem::getAccessor() {
  return (_dataAccessor);
}

SequenceDescriptor::SequenceDescriptor(
    const RegisterInfo_t& registerInfo, unsigned int sequenceNumber,
    const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, SequenceDescriptor::DataType, parent_),
      _registerMapElement(registerInfo),
      _sequenceNumber(sequenceNumber) {}

void SequenceDescriptor::read(TableWidgetData const& tabledata) {

  try {
  	MuxedData_t const& accessor = getAccessor();
    accessor->read();
    createTableRowEntries(tabledata, (*accessor)[0].size());
    putValuesIntoTable(tabledata, (*accessor)[_sequenceNumber]);
  }
  catch (...) {
    fillTableWithDummyValues(tabledata);
    throw;
  }

}

void SequenceDescriptor::write(TableWidgetData const& tabledata ) {
  	MuxedData_t const& accessor = getAccessor();
  	writeSequenceToCard(tabledata, accessor);
}

void SequenceDescriptor::updateRegisterProperties(
    const RegisterPropertyGrpBox& grpBox) {
	fillGrpBox(grpBox, _registerMapElement);
}

const RegisterInfo_t SequenceDescriptor::getRegisterMapElement() {
  return (_registerMapElement);
}

MuxedData_t const& SequenceDescriptor::getAccessor() {
  MultiplexedAreaItem* areaDescriptor;
  areaDescriptor = dynamic_cast<MultiplexedAreaItem*>(this->parent());
  if (!areaDescriptor) {
    throw InternalErrorException("could not find parent element");
  }
  return (areaDescriptor->getAccessor());
}

void CustomQTreeItem::fillGrpBox(const RegisterPropertyGrpBox& grpBox,
                                 const mtca4u::RegisterInfoMap::RegisterInfo& regInfo) {
  grpBox.registerNameDisplay->setText(regInfo.reg_name.c_str());
  grpBox.moduleDisplay->setText(regInfo.reg_module.c_str());
  grpBox.registerBarDisplay->setText(QString::number(regInfo.reg_bar));
  grpBox.registerNElementsDisplay->setText(
      QString::number(regInfo.reg_elem_nr));
  grpBox.registerAddressDisplay->setText(QString::number(regInfo.reg_address));
  grpBox.registerSizeDisplay->setText(QString::number(regInfo.reg_size));
  grpBox.registerWidthDisplay->setText(QString::number(regInfo.reg_width));
  grpBox.registerFracBitsDisplay->setText(
      QString::number(regInfo.reg_frac_bits));
  grpBox.registeSignBitDisplay->setText(QString::number(regInfo.reg_signed));
}

void SequenceDescriptor::writeSequenceToCard(const TableWidgetData& tabledata,
                                             MuxedData_t const& accessor) {
	unsigned int sequenceLength = (*accessor)[_sequenceNumber].size();
	(*accessor)[_sequenceNumber] = copyValuesFromTable<double>(tabledata, sequenceLength);
  accessor->write();
}

void ModuleItem::clearGrpBox(const RegisterPropertyGrpBox& grpBox) {
  grpBox.registerNameDisplay->setText("");
  grpBox.moduleDisplay->setText("");
  grpBox.registerBarDisplay->setText("");
  grpBox.registerNElementsDisplay->setText("");
  grpBox.registerAddressDisplay->setText("");
  grpBox.registerSizeDisplay->setText("");
  grpBox.registerWidthDisplay->setText("");
  grpBox.registerFracBitsDisplay->setText("");
  grpBox.registeSignBitDisplay->setText("");
}

