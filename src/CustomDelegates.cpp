/*
 * TableSpinBoxDelegate.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: varghese
 */

#include "CustomDelegates.h"
#include <QDoubleSpinBox>
#include <QStringBuilder>
#include <climits> // for the max and min integer values
#include <sstream>
#include <iostream>

const unsigned int CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION;

CustomDelegates::CustomDelegates(QObject *parent_)
    : QStyledItemDelegate(parent_),
      _doubleSpinBoxPrecision(DOUBLE_SPINBOX_DEFAULT_PRECISION) {}

/*
 * Method displays double values in the cell with decimal places. ((eg: 123.6500
 * if precision required is 4)). The desired number of decimal places
 * (precision) can be specified by setting
 * _doubleSpinBoxPrecision
 */
QString CustomDelegates::displayText(const QVariant &value,
                                     const QLocale &locale) const {
  if (value.type() == QVariant::Double) { // frame and return a formatted
                                          // QString if handling a cell with
                                          // double value
    return locale.toString((value.toDouble()), 'f', _doubleSpinBoxPrecision);
  } else if (value.type() == QVariant::UserType) {
    // The hex type is the only defined type at this point so this check should
    // be OK

    HexData hexValue =
        value.value<HexData>(); // get the datatype stored in this cell
    // QString::number has a bug and gives negative values as 64 bit hex.
    // So we have to do it manually.
    int intValue = hexValue.value; // covert the value to 32 bit int
    std::stringstream s;
    s << "0x" << std::hex << intValue;
    return QString(s.str().c_str());

  } else { // else let the default delegate take care of formatting if
           // content is not double
    return QStyledItemDelegate::displayText(value, locale);
  }
}

/*
 * Use a customized QDoubleSpinbox to modify cells with double value. The
 * spinbox supports a precision specified by _doubleSpinBoxPrecision.
 */
QWidget *CustomDelegates::createEditor(QWidget *parent_,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const {
  std::cout << "type is " << index.data(Qt::EditRole).type() << std::endl;
  if (index.data(Qt::EditRole).type() ==
      QVariant::Double) { // create spinbox with custom precision
                          // for cells in the double column
    QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox(parent_);
    doubleSpinBox->setDecimals(_doubleSpinBoxPrecision);
    return doubleSpinBox;
  } else if (index.data(Qt::EditRole).type() == QVariant::UserType) {
    // TODO: make the check above specific to our custom HexDataType
    HexSpinBox *hexSpinBox = new HexSpinBox(parent_);
    hexSpinBox->setRange(INT_MIN, INT_MAX);
    return hexSpinBox;
  } else {
    // should work for int, uint, string and bool
    std::cout << "creating default delegate " << std::endl;
    return QStyledItemDelegate::createEditor(parent_, option, index);
  }
}

void CustomDelegates::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const {
  if (index.data(Qt::EditRole).type() == QVariant::UserType) {
    HexData hexData = index.data(Qt::EditRole).value<HexData>();
    HexSpinBox *spinBox = static_cast<HexSpinBox *>(editor);
    spinBox->setValue(hexData.value);
  } else {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

void CustomDelegates::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const {
  if (index.data(Qt::EditRole).type() == QVariant::UserType) {
    HexSpinBox *spinBox = static_cast<HexSpinBox *>(editor);
    spinBox->interpretText();
    double value = spinBox->value();
    HexData hexData;
    hexData.value = value;
    QVariant dataVariant;
    dataVariant.setValue(hexData);
    model->setData(index, dataVariant, Qt::EditRole);
  } else {
    QStyledItemDelegate::setModelData(editor, model, index);
  }
}

void CustomDelegates::setDoubleSpinBoxPrecision(
    unsigned int doubleSpinBoxPrecision) {
  _doubleSpinBoxPrecision = static_cast<int>(doubleSpinBoxPrecision);
}
