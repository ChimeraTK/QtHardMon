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

void ModuleItem::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

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
