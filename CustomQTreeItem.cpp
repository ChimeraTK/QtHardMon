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

mtca4u::mapFile::mapElem CustomQTreeItem::getRegisterMapElement() {
  return (mtca4u::mapFile::mapElem());
}

CustomQTreeItem::~CustomQTreeItem() {
  // TODO Auto-generated destructor stub
}
