/*
 * customQTreeItemInterface.cpp
 *
 *  Created on: Jul 14, 2015
 */

#include "CustomQTreeItem.h"
#include <boost/shared_ptr.hpp>
#include <MtcaMappedDevice/devBase.h>

typedef boost::shared_ptr< mtca4u::MultiplexedDataAccessor<double> > MuxedData;

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidget* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

CustomQTreeItem::CustomQTreeItem(const QString& text_, const int type_,
                                 QTreeWidgetItem* parent_)
    : QTreeWidgetItem(parent_, QStringList(text_), type_) {}

mtca4u::mapFile::mapElem const CustomQTreeItem::getRegisterMapElement() {
  return (mtca4u::mapFile::mapElem());
}

CustomQTreeItem::~CustomQTreeItem() {
  // TODO Auto-generated destructor stub
}

ModuleItem::ModuleItem(const QString& text_, QTreeWidget* parent_)
    : CustomQTreeItem(text_, ModuleItem::DataType, parent_) {}

void ModuleItem::read(QTableWidget* const tablewidget,
                      mtca4u::devPCIE const& device) {}

void ModuleItem::write(QTableWidget* const tablewidget,
                       mtca4u::devPCIE const& device) {}

void ModuleItem::updateRegisterProperties(const RegsterPropertyGrpBox& grpBox) {
}

RegisterItem::RegisterItem(const mtca4u::mapFile::mapElem& registerInfo,
                           const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, RegisterItem::DataType, parent_),
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
    boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const& accessor,
    const mtca4u::mapFile::mapElem& registerInfo, const QString& text_,
    QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, MultiplexedAreaItem::DataType, parent_),
      _dataAccessor(accessor),
      _registerMapElement(registerInfo) {}

void MultiplexedAreaItem::read(QTableWidget* const tablewidget,
                               mtca4u::devPCIE const& device) {}

void MultiplexedAreaItem::write(QTableWidget* const tablewidget,
                                mtca4u::devPCIE const& device) {}

void MultiplexedAreaItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const mtca4u::mapFile::mapElem MultiplexedAreaItem::getRegisterMapElement() {
  return (_registerMapElement);
}
const boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> >&
MultiplexedAreaItem::getAccessor() {
	return (_dataAccessor);
}

SequenceDescriptor::SequenceDescriptor(
    const mtca4u::mapFile::mapElem& registerInfo, unsigned int sequenceNumber,
    const QString& text_, QTreeWidgetItem* parent_)
    : CustomQTreeItem(text_, SequenceDescriptor::DataType, parent_),
      _registerMapElement(registerInfo),
      _sequenceNumber(sequenceNumber) {}

void SequenceDescriptor::read(QTableWidget* const tablewidget,
                              mtca4u::devPCIE const& device) {
  MuxedData const& accessor = getAccessor();

}

void SequenceDescriptor::write(QTableWidget* const tablewidget,
                               mtca4u::devPCIE const& device) {}

void SequenceDescriptor::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const mtca4u::mapFile::mapElem SequenceDescriptor::getRegisterMapElement() {
  return (_registerMapElement);
}

const boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> >&
SequenceDescriptor::getAccessor() {
	MultiplexedAreaItem* parent;
	parent = dynamic_cast<MultiplexedAreaItem* >(this->parent());
	if(!parent){
			throw;
	}
	return(parent->getAccessor());
}
