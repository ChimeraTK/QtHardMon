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

bool CustomQTreeItem::operator<(const QTreeWidgetItem& rhs) const {
  QRegExp checkForNumAtEnd("[0-9]+$");

  QString lhsName = this->text(0);
  QString rhsName = rhs.text(0);
  int lhsIndex = checkForNumAtEnd.indexIn(lhsName);
  int rhsIndex = checkForNumAtEnd.indexIn(rhsName);
  QString lhsNameTextPart = lhsName.mid(0, lhsIndex);
  QString rhsNameTextPart = rhsName.mid(0, rhsIndex);

  if ((lhsNameTextPart != rhsNameTextPart) || (lhsIndex == -1) ||
      (rhsIndex == -1)) { // Index is -1 when the QTreeWidgetItem does not end
                          // in a numeric value
    return QTreeWidgetItem::operator<(rhs);
  } else {
    int lhsNumericalPart =
        lhsName.mid(lhsIndex).toInt(); // convert substring from lhsIndex to
                                       // end of string to int
    int rhsNumericalPart = rhsName.mid(rhsIndex).toInt();
    return lhsNumericalPart < rhsNumericalPart;
  }
}

CustomQTreeItem::~CustomQTreeItem() {
  // TODO Auto-generated destructor stub
}

ModuleItem::ModuleItem(const QString& text_, QTreeWidget* parent_)
    : CustomQTreeItem(text_, ModuleItem::DataType, parent_) {}

void ModuleItem::read(TableWidgetData const& tabledata) {
  clearTable(tabledata);
  throw InvalidOperationException(
      "You cannot read from a module. Select a register.");
}

void ModuleItem::write(TableWidgetData const& /*tabledata*/) {
  throw InvalidOperationException(
      "You cannot write from a module. Select a register.");
}

void ModuleItem::updateRegisterProperties(
    const RegisterPropertyGrpBox& grpBox) {
  clearGrpBox(grpBox);
}

RegisterItem::RegisterItem(const RegisterInfo_t& registerInfo,
                           const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, RegisterItem::DataType, parent_),
      _registerMapElement(registerInfo),
      _fixedPointConverter(registerInfo.module+"/"+registerInfo.name,
                           registerInfo.width, registerInfo.nFractionalBits,
                           registerInfo.signedFlag) {}

void RegisterItem::read(TableWidgetData const& tabledata) {
  std::vector<int> inputBuffer = fetchElementsFromCard(tabledata);
  putValuesIntoTable(tabledata, inputBuffer);
}

std::vector<int> RegisterItem::fetchElementsFromCard(
    const TableWidgetData& tabledata) {
  RegisterInfo_t regInfo = this->getRegisterMapElement();

  int numberOfElements = regInfo.nElements;
  std::vector<int> buffer(numberOfElements);

  unsigned int registerBar = regInfo.bar;
  Device_t const& mtcadevice = tabledata.device;
  int maxrows = tabledata.tableMaxRowCount;
  size_t nBytesToRead =
      std::min(numberOfElements, maxrows) * qthardmon::WORD_SIZE_IN_BYTES;
  unsigned int registerAddress = regInfo.address;

  mtcadevice->read(registerBar, registerAddress, &(buffer[0]), nBytesToRead);
  return buffer;
}

void RegisterItem::write(TableWidgetData const& tabledata) {
  unsigned int numElementsinRegister = _registerMapElement.nElements;
  std::vector<int> buffer =
      copyValuesFromTable<int>(tabledata, numElementsinRegister);
  writeRegisterToDevice(tabledata, buffer);
}

const mtca4u::FixedPointConverter RegisterItem::getFixedPointConverter() {
  return _fixedPointConverter;
}
void RegisterItem::writeRegisterToDevice(TableWidgetData const& tabledata,
                                         const std::vector<int>& buffer) {
  Device_t const& mtcadevice = tabledata.device;
  unsigned int regAddress = _registerMapElement.address;
  unsigned int regSizeinBytes = _registerMapElement.nBytes;
  unsigned int register bar = _registerMapElement.bar;
  mtcadevice->write(bar, regAddress, &buffer[0], regSizeinBytes);
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
  clearTable(tabledata);
}

void MultiplexedAreaItem::write(TableWidgetData const& /*tabledata*/) {
  // do nothing? -> change behavior later to something better?
}

void MultiplexedAreaItem::updateRegisterProperties(
    const RegisterPropertyGrpBox& grpBox) {
  grpBox.registerNameDisplay->setText(_registerMapElement.name.c_str());
  grpBox.moduleDisplay->setText(_registerMapElement.module.c_str());
  grpBox.registerBarDisplay->setText(QString::number(_registerMapElement.bar));
  grpBox.registerNElementsDisplay->setText("");
  grpBox.registerAddressDisplay->setText(
      QString::number(_registerMapElement.address));
  grpBox.registerSizeDisplay->setText(
      QString::number(_registerMapElement.nBytes));

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

SequenceDescriptor::SequenceDescriptor(const RegisterInfo_t& registerInfo,
                                       unsigned int sequenceNumber,
                                       const QString& text_,
                                       QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, SequenceDescriptor::DataType, parent_),
      _registerMapElement(registerInfo),
      _fixedPointConverter(registerInfo.module+"/"+registerInfo.name,
                           registerInfo.width, registerInfo.nFractionalBits,
                           registerInfo.signedFlag),
      _sequenceNumber(sequenceNumber) {}

void SequenceDescriptor::read(TableWidgetData const& tabledata) {
  MuxedData_t const& accessor = getAccessor();
  accessor->read();
  putValuesIntoTable(tabledata, (*accessor)[_sequenceNumber]);
}

void SequenceDescriptor::write(TableWidgetData const& tabledata) {
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

void CustomQTreeItem::clearTable(const TableWidgetData& tabledata) {
  QTableWidget* table = tabledata.table;
  table->clearContents();
  table->setRowCount(0);
}

void CustomQTreeItem::fillGrpBox(
    const RegisterPropertyGrpBox& grpBox,
    const mtca4u::RegisterInfoMap::RegisterInfo& regInfo) {
  grpBox.registerNameDisplay->setText(regInfo.name.c_str());
  grpBox.moduleDisplay->setText(regInfo.module.c_str());
  grpBox.registerBarDisplay->setText(QString::number(regInfo.bar));
  grpBox.registerNElementsDisplay->setText(QString::number(regInfo.nElements));
  grpBox.registerAddressDisplay->setText(QString::number(regInfo.address));
  grpBox.registerSizeDisplay->setText(QString::number(regInfo.nBytes));
  grpBox.registerWidthDisplay->setText(QString::number(regInfo.width));
  grpBox.registerFracBitsDisplay->setText(
      QString::number(regInfo.nFractionalBits));
  grpBox.registeSignBitDisplay->setText(QString::number(regInfo.signedFlag));
}

const mtca4u::FixedPointConverter SequenceDescriptor::getFixedPointConverter() {
  return _fixedPointConverter;
}

void SequenceDescriptor::writeSequenceToCard(const TableWidgetData& tabledata,
                                             MuxedData_t const& accessor) {
  unsigned int sequenceLength = (*accessor)[_sequenceNumber].size();
  (*accessor)[_sequenceNumber] =
      copyValuesFromTable<double>(tabledata, sequenceLength);
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

const mtca4u::FixedPointConverter CustomQTreeItem::getFixedPointConverter() {
  // The default implementation provides a 'dummy' fixed point converter with
  // the default settings
  RegisterInfo_t regInfo = this->getRegisterMapElement();
  return mtca4u::FixedPointConverter(regInfo.module+"/"+regInfo.name,
                                     regInfo.width, regInfo.nFractionalBits,
                                     regInfo.signedFlag);
}
