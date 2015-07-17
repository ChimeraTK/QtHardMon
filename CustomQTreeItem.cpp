/*
 * customQTreeItemInterface.cpp
 *
 *  Created on: Jul 14, 2015
 */

#include "CustomQTreeItem.h"
#include <boost/shared_ptr.hpp>
#include <MtcaMappedDevice/devBase.h>
#include "Exceptions.h"

typedef boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > MuxedData;
typedef boost::shared_ptr<mtca4u::devBase> Device_t;
typedef mtca4u::mapFile::mapElem RegisterInfo;

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidget* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidgetItem* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

RegisterInfo const CustomQTreeItem::getRegisterMapElement() {
  return (RegisterInfo());
}

CustomQTreeItem::~CustomQTreeItem() {
  // TODO Auto-generated destructor stub
}
void CustomQTreeItem::fillTableWithDummyValues(
    const TableWidgetData& tableData) {

	// FIXME: Take care of boilerplate
  RegisterInfo regInfo = this->getRegisterMapElement();
  unsigned int numElements = regInfo.reg_elem_nr;

  QTableWidget* table = tableData.table;
  unsigned int maxRow = tableData.tableMaxRowCount;

  for (unsigned int row = 0; row < numElements; row++) {
    QTableWidgetItem* dataItem = new QTableWidgetItem();
    if (row == maxRow) {
      dataItem->setText("truncated");
      dataItem->setFlags(dataItem->flags() & ~Qt::ItemIsSelectable &
                         ~Qt::ItemIsEditable);
      dataItem->setToolTip("List is truncated. You can change the number of "
                           "words displayed in the preferences.");
      table->setItem(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN, dataItem);
      break;
    }

    int registerContent = -1;

    dataItem->setData(0, QVariant(registerContent)); // 0 is the default role
    //table->setItem(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN, dataItem);
    table->setItem(row, 0, dataItem);
  }
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

void ModuleItem::read(TableWidgetData const& tabledata) {}

void ModuleItem::write(TableWidgetData const& tabledata) {}

void ModuleItem::updateRegisterProperties(const RegsterPropertyGrpBox& grpBox) {
}

RegisterItem::RegisterItem(const RegisterInfo& registerInfo,
                           const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, RegisterItem::DataType, parent_),
      _registerMapElement(registerInfo) {}

void RegisterItem::read(TableWidgetData const& tabledata) {
  try {

  		std::vector<int> inputBuffer = fetchElementsFromCard(tabledata);
  		createTableRowEntries(tabledata, inputBuffer.size());
  		//putValuesIntoTable(tabledata, accessor);*/
  }
  catch (...) {
    fillTableWithDummyValues(tabledata);
    throw;
  }
}

std::vector<int> RegisterItem::fetchElementsFromCard(
    const TableWidgetData& tabledata) {
  RegisterInfo regInfo = this->getRegisterMapElement();

  int numberOfElements = regInfo.reg_elem_nr;
  std::vector<int> buffer(numberOfElements);

  unsigned int registerBar = regInfo.reg_bar;
  Device_t const& mtcadevice = tabledata.device;
  int maxrows = tabledata.tableMaxRowCount;
  size_t nBytesToRead =
      std::min(numberOfElements, maxrows) * qthardmon::WORD_SIZE_IN_BYTES;
  unsigned int registerAddress = regInfo.reg_address;

  if (registerBar == 0xD) {
    mtcadevice->readDMA(registerAddress, &(buffer[0]), nBytesToRead,
                        registerBar);
  } else {
    mtcadevice->readArea(registerAddress, &(buffer[0]), nBytesToRead,
                        registerBar);
  }
  return buffer;
}

void RegisterItem::write(TableWidgetData const& tabledata) {}

void RegisterItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const RegisterInfo RegisterItem::getRegisterMapElement() {
  return _registerMapElement;
}

MultiplexedAreaItem::MultiplexedAreaItem(
    boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const& accessor,
    const RegisterInfo& registerInfo, const QString& text_,
    QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, MultiplexedAreaItem::DataType, parent_),
      _dataAccessor(accessor),
      _registerMapElement(registerInfo) {}

void MultiplexedAreaItem::read(TableWidgetData const& tabledata) {
	// Nothing to read
	createTableRowEntries(tabledata, 0);
}

void MultiplexedAreaItem::write(TableWidgetData const& tabledata) {}

void MultiplexedAreaItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const RegisterInfo MultiplexedAreaItem::getRegisterMapElement() {
  return (_registerMapElement);
}
boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
MultiplexedAreaItem::getAccessor() {
  return (_dataAccessor);
}

SequenceDescriptor::SequenceDescriptor(
    const RegisterInfo& registerInfo, unsigned int sequenceNumber,
    const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, SequenceDescriptor::DataType, parent_),
      _registerMapElement(registerInfo),
      _sequenceNumber(sequenceNumber) {}

void SequenceDescriptor::read(TableWidgetData const& tabledata) {

  try {
  	MuxedData const& accessor = getAccessor();
    accessor->read();
    createTableRowEntries(tabledata, (*accessor)[0].size());
    putValuesIntoTable<double>(tabledata, (*accessor)[_sequenceNumber]);
  }
  catch (...) {
    fillTableWithDummyValues(tabledata);
    throw;
  }

}

void SequenceDescriptor::write(TableWidgetData const& tabledata ) {}

void SequenceDescriptor::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const RegisterInfo SequenceDescriptor::getRegisterMapElement() {
  return (_registerMapElement);
}

MuxedData const& SequenceDescriptor::getAccessor() {
  MultiplexedAreaItem* areaDescriptor;
  areaDescriptor = dynamic_cast<MultiplexedAreaItem*>(this->parent());
  if (!areaDescriptor) {
    throw InternalErrorException("could not find parent element");
  }
  return (areaDescriptor->getAccessor());
}

/*void SequenceDescriptor::putValuesIntoTable(const TableWidgetData& tabledata,
                                            const MuxedData& accessor) {
  QTableWidget* table = tabledata.table;
  unsigned int maxRow = tabledata.tableMaxRowCount;

  for (unsigned int row=0; row < (*accessor)[0].size(); row++)
  {
    // Prepare a data item with a QVariant. The QVariant takes care that the type is recognised as
    // int and a proper editor (spin box) is used when editing in the GUI.
    QTableWidgetItem * dataItem =  new QTableWidgetItem();

    if (row == maxRow)
    { // The register is too large to display. Show that it is truncated and stop reading
      dataItem->setText("truncated");
      dataItem->setFlags( dataItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable );
      dataItem->setToolTip("List is truncated. You can change the number of words displayed in the preferences.");
      table->setItem(row, 0, dataItem );
      break;
    }
    //int registerContent = (readError?-1:inputBuffer[row]);
    int registerContent = (*accessor)[_sequenceNumber][row];

    dataItem->setData( 0, QVariant( registerContent ) ); // 0 is the default role
    table->setItem(row, qthardmon::FLOATING_POINT_DISPLAY_COLUMN, dataItem );
  }// for row
}*/


