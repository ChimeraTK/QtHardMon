/*
 * customQTreeItemInterface.cpp
 *
 *  Created on: Jul 14, 2015
 *      Author: varghese
 */

#include "CustomQTreeItem.h"

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

void ModuleItem::read(QTableWidget* const tablewidget) {}

void ModuleItem::write(QTableWidget* const tablewidget) {}

void ModuleItem::updateRegisterProperties(const RegsterPropertyGrpBox& grpBox) {
}

RegisterItem::RegisterItem(const mtca4u::mapFile::mapElem& registerInfo,
                           QTreeWidgetItem* parent_, const QString& text_)
    : CustomQTreeItem(parent_, text_, RegisterItem::DataType),
      _registerMapElement(registerInfo) {}

void RegisterItem::read(QTableWidget* const tablewidget) {}

void RegisterItem::write(QTableWidget* const tablewidget) {}

void RegisterItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const mtca4u::mapFile::mapElem RegisterItem::getRegisterMapElement() {
  return _registerMapElement;
}

MultiplexedAreaItem::MultiplexedAreaItem(
		mtca4u::ptrmapFile const & ptrmapFile,
    const mtca4u::mapFile::mapElem& registerInfo, QTreeWidgetItem* parent_,
    const QString& text_)
    : CustomQTreeItem(parent_, text_, MultiplexedAreaItem::DataType),
      _ptrmapFile(ptrmapFile),
      _registerMapElement(registerInfo) {}

void MultiplexedAreaItem::read(QTableWidget* const tablewidget) {}

void MultiplexedAreaItem::write(QTableWidget* const tablewidget) {}

void MultiplexedAreaItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

SequenceDescriptor::SequenceDescriptor(
    const mtca4u::mapFile::mapElem& registerInfo, QTreeWidgetItem* parent_,
    const QString& text_)
    : CustomQTreeItem(parent_, text_, SequenceDescriptor::DataType) {}

void SequenceDescriptor::read(QTableWidget* const tablewidget) {}

void SequenceDescriptor::write(QTableWidget* const tablewidget) {}

void SequenceDescriptor::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

const mtca4u::mapFile::mapElem SequenceDescriptor::getRegisterMapElement() {
  return (_registerMapElement);
}
