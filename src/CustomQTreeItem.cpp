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

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidget* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidgetItem* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

const mtca4u::RegisterInfo * CustomQTreeItem::getRegisterMapElement() {
  return new mtca4u::RegisterInfoMap::RegisterInfo();
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

RegisterItem::RegisterItem(const mtca4u::RegisterInfo* registerInfo,
                           const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, RegisterItem::DataType, parent_),
      _registerMapElement(registerInfo),
      _fixedPointConverter(registerInfo->getRegisterName().getComponents().front()+"/"+registerInfo->getRegisterName().getComponents().back(),
                           dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(registerInfo)->width, dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(registerInfo)->nFractionalBits,
                           dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(registerInfo)->signedFlag) {}

void RegisterItem::read(TableWidgetData const& tabledata) {
  std::vector<int> inputBuffer = fetchElementsFromCard(tabledata);
  putValuesIntoTable(tabledata, inputBuffer);
}

std::vector<int> RegisterItem::fetchElementsFromCard(
    const TableWidgetData& tabledata) {
  const mtca4u::RegisterInfo * regInfo = this->getRegisterMapElement();

  int numberOfElements = regInfo->getNumberOfElements();
  std::vector<int> buffer(numberOfElements);

  unsigned int registerBar = dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->bar;
  int maxrows = tabledata.tableMaxRowCount;
  size_t nBytesToRead =
      std::min(numberOfElements, maxrows) * qthardmon::WORD_SIZE_IN_BYTES;
  unsigned int registerAddress = dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->address;
  #warning broken functionality of read.
  //tabledata.device.read(registerBar, registerAddress, &(buffer[0]), nBytesToRead);
  return buffer;
}

void RegisterItem::write(TableWidgetData const& tabledata) {
  unsigned int numElementsinRegister = _registerMapElement->getNumberOfElements();
  std::vector<int> buffer =
      copyValuesFromTable<int>(tabledata, numElementsinRegister);
  writeRegisterToDevice(tabledata, buffer);
}

const mtca4u::FixedPointConverter RegisterItem::getFixedPointConverter() {
  return _fixedPointConverter;
}
void RegisterItem::writeRegisterToDevice(TableWidgetData const& tabledata,
                                         const std::vector<int>& buffer) {
  unsigned int regAddress = dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(_registerMapElement)->address;
  unsigned int regSizeinBytes = dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(_registerMapElement)->nBytes;
  unsigned int register bar = dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(_registerMapElement)->bar;
  #warning broken functionality of write.
  // tabledata.device.write(bar, regAddress, &buffer[0], regSizeinBytes);
}

void RegisterItem::updateRegisterProperties(
    const RegisterPropertyGrpBox& grpBox) {
  fillGrpBox(grpBox, _registerMapElement);
}

const mtca4u::RegisterInfo * RegisterItem::getRegisterMapElement() {
  return _registerMapElement;
}

MultiplexedAreaItem::MultiplexedAreaItem(
    boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const& accessor,
    const mtca4u::RegisterInfo* registerInfo, const QString& text_,
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
  grpBox.registerNameDisplay->setText(_registerMapElement->getRegisterName().getComponents().back().c_str());
  grpBox.moduleDisplay->setText(_registerMapElement->getRegisterName().getComponents().front().c_str());
  grpBox.registerBarDisplay->setText(QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(_registerMapElement)->bar));
  grpBox.registerNElementsDisplay->setText("");
  grpBox.registerAddressDisplay->setText(
      QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(_registerMapElement)->address));
  grpBox.registerSizeDisplay->setText(
      QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(_registerMapElement)->nBytes));

  grpBox.registerWidthDisplay->setText("");
  grpBox.registerFracBitsDisplay->setText("");
  grpBox.registeSignBitDisplay->setText("");
}

const mtca4u::RegisterInfo * MultiplexedAreaItem::getRegisterMapElement() {
  return (_registerMapElement);
}
boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
MultiplexedAreaItem::getAccessor() {
  return (_dataAccessor);
}

SequenceDescriptor::SequenceDescriptor(const mtca4u::RegisterInfo* registerInfo,
                                       unsigned int sequenceNumber,
                                       const QString& text_,
                                       QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, SequenceDescriptor::DataType, parent_),
      _registerMapElement(registerInfo),
      _fixedPointConverter(registerInfo->getRegisterName().getComponents().front()+"/"+registerInfo->getRegisterName().getComponents().back(),
                           dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(registerInfo)->width, dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(registerInfo)->nFractionalBits,
                           dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(registerInfo)->signedFlag),
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

const mtca4u::RegisterInfo  * SequenceDescriptor::getRegisterMapElement() {
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
    const mtca4u::RegisterInfo * regInfo) {
  grpBox.registerNameDisplay->setText(regInfo->getRegisterName().getComponents().back().c_str());
  grpBox.moduleDisplay->setText(regInfo->getRegisterName().getComponents().front().c_str());
  grpBox.registerBarDisplay->setText(QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->bar));
  grpBox.registerNElementsDisplay->setText(QString::number(regInfo->getNumberOfElements()));
  grpBox.registerAddressDisplay->setText(QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->address));
  grpBox.registerSizeDisplay->setText(QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->nBytes));
  grpBox.registerWidthDisplay->setText(QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->width));
  grpBox.registerFracBitsDisplay->setText(
      QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->nFractionalBits));
  grpBox.registeSignBitDisplay->setText(QString::number(dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->signedFlag));
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
  const mtca4u::RegisterInfo * regInfo = getRegisterMapElement();
  return mtca4u::FixedPointConverter(regInfo->getRegisterName().getComponents().front()+"/"+regInfo->getRegisterName().getComponents().back(),
                                     dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->width, dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->nFractionalBits,
                                     dynamic_cast<const mtca4u::RegisterInfoMap::RegisterInfo*>(regInfo)->signedFlag);
}
