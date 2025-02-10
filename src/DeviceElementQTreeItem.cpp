#include "DeviceElementQTreeItem.h"

DeviceElementQTreeItem::DeviceElementQTreeItem(
    QTreeWidget* parent, const QString& text, const ChimeraTK::RegisterInfo& registerInfo)
: QTreeWidgetItem(parent, QStringList(text), QTreeWidgetItem::UserType), registerInfo_(registerInfo) {}

DeviceElementQTreeItem::DeviceElementQTreeItem(
    QTreeWidgetItem* parent, const QString& text, const ChimeraTK::RegisterInfo& registerInfo)
: QTreeWidgetItem(parent, QStringList(text), QTreeWidgetItem::UserType), registerInfo_(registerInfo) {}

bool DeviceElementQTreeItem::operator<(const QTreeWidgetItem& rhs) const {
  QRegularExpression checkForNumAtEnd("[0-9]+$");

  QString lhsName = this->text(0);
  QString rhsName = rhs.text(0);
  int lhsIndex = lhsName.indexOf(checkForNumAtEnd);
  int rhsIndex = rhsName.indexOf(checkForNumAtEnd);
  QString lhsNameTextPart = lhsName.mid(0, lhsIndex);
  QString rhsNameTextPart = rhsName.mid(0, rhsIndex);

  if((lhsNameTextPart != rhsNameTextPart) || (lhsIndex == -1) || (rhsIndex == -1)) { // Index is -1 when the
                                                                                     // QTreeWidgetItem does not end in
                                                                                     // a numeric value
    return QTreeWidgetItem::operator<(rhs);
  }
  else {
    int lhsNumericalPart = lhsName.mid(lhsIndex).toInt(); // convert substring from lhsIndex to
                                                          // end of string to int
    int rhsNumericalPart = rhsName.mid(rhsIndex).toInt();
    return lhsNumericalPart < rhsNumericalPart;
  }
}

const ChimeraTK::RegisterInfo& DeviceElementQTreeItem::getRegisterInfo() {
  return registerInfo_;
}

ChimeraTK::RegisterPath DeviceElementQTreeItem::getRegisterPath() {
  // start (or actially end) the register path with the text label of this item.
  ChimeraTK::RegisterPath returnValue = text(0).toStdString();
  // go up the tree until there is no parent, and prepend the parent names to
  // the path
  auto theParent = this->parent();
  while(theParent) {
    returnValue = ChimeraTK::RegisterPath(theParent->text(0).toStdString()) / returnValue;
    theParent = theParent->parent();
  }
  return returnValue;
}
