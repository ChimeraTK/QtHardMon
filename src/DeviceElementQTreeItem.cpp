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

bool DeviceElementQTreeItem::operator<(const QTreeWidgetItem& rhs) const {
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