#include "DeviceElementQTreeItem.h"

DeviceElementQTreeItem::DeviceElementQTreeItem(const QString & text, 
const int type, QTreeWidget * parent)
: QTreeWidgetItem(parent, QStringList(text), type) 
{   
}

DeviceElementQTreeItem::DeviceElementQTreeItem(const QString & text, 
const int type, QTreeWidgetItem * parent)
: QTreeWidgetItem(parent, QStringList(text), type) 
{
}
