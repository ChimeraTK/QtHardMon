/*
 * customQTreeItemInterface.cpp
 *
 *  Created on: Jul 14, 2015
 */

#include "CustomQTreeItem.h"
#include <boost/shared_ptr.hpp>
#include <MtcaMappedDevice/devBase.h>

CustomQTreeItem::CustomQTreeItem(QTreeWidget* parent_, const QString& text_,
                                 const int type_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

CustomQTreeItem::CustomQTreeItem(QTreeWidgetItem* parent_, const QString& text_,
                                 const int type_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

mtca4u::mapFile::mapElem const CustomQTreeItem::getRegisterMapElement() {
  return (mtca4u::mapFile::mapElem());
}

CustomQTreeItem::~CustomQTreeItem() {
  // TODO Auto-generated destructor stub
}

ModuleItem::ModuleItem(QTreeWidget* parent_, const QString& text_)
    : CustomQTreeItem(parent_, text_, ModuleItem::DataType) {}

void ModuleItem::read(QTableWidget* const tablewidget,
                      mtca4u::devPCIE const& device) {}

void ModuleItem::write(QTableWidget* const tablewidget,
                       mtca4u::devPCIE const& device) {}

void ModuleItem::updateRegisterProperties(const RegsterPropertyGrpBox& grpBox) {
}

RegisterItem::RegisterItem(const mtca4u::mapFile::mapElem& registerInfo,
                           QTreeWidgetItem* parent_, const QString& text_)
    : CustomQTreeItem(parent_, text_, RegisterItem::DataType),
      _registerMapElement(registerInfo) {}

void RegisterItem::read(QTableWidget* const tablewidget,
                        mtca4u::devPCIE const& device) {}

void RegisterItem::write(QTableWidget* const tablewidget,
                         mtca4u::devPCIE const& device) {}

void RegisterItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const mtca4u::mapFile::mapElem RegisterItem::getRegisterMapElement() {
  return _registerMapElement;
}

MultiplexedAreaItem::MultiplexedAreaItem(
    mtca4u::ptrmapFile const& ptrmapFile,
    const mtca4u::mapFile::mapElem& registerInfo, QTreeWidgetItem* parent_,
    const QString& text_)
    : CustomQTreeItem(parent_, text_, MultiplexedAreaItem::DataType),
      _ptrmapFile(ptrmapFile),
      _registerMapElement(registerInfo) {}

void MultiplexedAreaItem::read(QTableWidget* const tablewidget,
                               mtca4u::devPCIE const& device) {}

void MultiplexedAreaItem::write(QTableWidget* const tablewidget,
                                mtca4u::devPCIE const& device) {}

void MultiplexedAreaItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

mtca4u::ptrmapFile const& MultiplexedAreaItem::getPtrToMapFile() {
  return _ptrmapFile;
}

const mtca4u::mapFile::mapElem MultiplexedAreaItem::getRegisterMapElement() {
  return (_registerMapElement);
}

SequenceDescriptor::SequenceDescriptor(
    const mtca4u::mapFile::mapElem& registerInfo, unsigned int sequenceNumber,
    QTreeWidgetItem* parent_, const QString& text_)
    : CustomQTreeItem(parent_, text_, SequenceDescriptor::DataType),
      _registerMapElement(registerInfo),
      _sequenceNumber(sequenceNumber) {}

void SequenceDescriptor::read(QTableWidget* const tablewidget,
                              mtca4u::devPCIE const& device) {
  // FIXME: do not want to be creating demultiplexed accessor each time; wanted
  // this to be a member of the parent MultiplexedAreaItem class, but the way
  // the
  // things are currently structured prevents this. For now using this hack
  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > accessor =
      createAccessor(device);
  accessor->read();

  for (unsigned int row=0; row < (*accessor)[0].size(); row++)
  {
    // Prepare a data item with a QVariant. The QVariant takes care that the type is recognised as
    // int and a proper editor (spin box) is used when editing in the GUI.
    QTableWidgetItem * dataItem =  new QTableWidgetItem();
/*
    if (row == _maxWords)
    { // The register is too large to display. Show that it is truncated and stop reading
      dataItem->setText("truncated");
      // turn off the editable and selectable flags for the "truncated" entry
      dataItem->setFlags( dataItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable );
      dataItem->setToolTip("List is truncated. You can change the number of words displayed in the preferences.");
      _hardMonForm.valuesTableWidget->setItem(row, 0, dataItem );
      // no need to set the hex item
      break;
    }*/
    int registerContent = (*accessor)[_sequenceNumber][row];

    dataItem->setData( 0, QVariant( registerContent ) ); // 0 is the default role
    tablewidget->setItem(row, 3, dataItem );

  }// for row

}

void SequenceDescriptor::write(QTableWidget* const tablewidget,
                               mtca4u::devPCIE const& device) {}

void SequenceDescriptor::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const mtca4u::mapFile::mapElem SequenceDescriptor::getRegisterMapElement() {
  return (_registerMapElement);
}

boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> >
SequenceDescriptor::createAccessor(mtca4u::devPCIE const& device) {
  MultiplexedAreaItem* areaDescriptor =
      dynamic_cast<MultiplexedAreaItem*>(this->parent());
  if (!areaDescriptor) {
    throw; // Should not happen when hooked to the QtreeWidget
  }
  mtca4u::mapFile::mapElem areaDetails =
      areaDescriptor->getRegisterMapElement();
  std::string multiplexedSequenceName = getMuxAreaName(areaDetails.reg_name);
  std::string moduleName = areaDetails.reg_module;
  boost::shared_ptr<mtca4u::mapFile> const& registerMapping =
      areaDescriptor->getPtrToMapFile();
  boost::shared_ptr<mtca4u::devBase> iodevice(new mtca4u::devPCIE(device));

  return (mtca4u::MultiplexedDataAccessor<double>::createInstance(
      multiplexedSequenceName, moduleName, iodevice, registerMapping));
}

std::string SequenceDescriptor::getMuxAreaName(
    const std::string& registerName) {
  if (registerName.substr(0, mtca4u::MULTIPLEXED_SEQUENCE_PREFIX.size()) ==
      mtca4u::MULTIPLEXED_SEQUENCE_PREFIX) {
    return registerName.substr(mtca4u::MULTIPLEXED_SEQUENCE_PREFIX.size());
  }
  return "";
}
