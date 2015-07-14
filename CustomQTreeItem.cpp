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

ModuleEntry::ModuleEntry(QTreeWidget* parent_, const QString& text_)
    : CustomQTreeItem(parent_, text_, ModuleEntry::DataType) {}

void ModuleEntry::read(QTableWidget* const tablewidget) {}

void ModuleEntry::write(QTableWidget* const tablewidget) {}

void ModuleEntry::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}

RegisterEntry::RegisterEntry(mtca4u::mapFile::mapElem registerInfo,
                             QTreeWidgetItem* parent_, const QString& text_)
    : CustomQTreeItem(parent_, text_, RegisterEntry::DataType),
      _registerMapElement(registerInfo) {}

void RegisterEntry::read(QTableWidget* const tablewidget) {}

void RegisterEntry::write(QTableWidget* const tablewidget) {}

void RegisterEntry::updateRegisterProperties(
    const RegsterPropertyGrpBox& grpBox) {}
