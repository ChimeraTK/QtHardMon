/*
 * TableSpinBoxDelegate.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: varghese
 */

#include "TableSpinBoxDelegate.h"
#include <QDoubleSpinBox>



TableSpinBoxDelegate::TableSpinBoxDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      _doubleSpinBoxPrecision(DOUBLE_SPINBOX_DEFAULT_PRECISION)
      {}

/*
 * Method displays double values in the cell with decimal places. ((eg: 123.6500
 * if precision required is 4)). The desired number of decimal places
 * (precision) can be specified by setting
 * _doubleSpinBoxPrecision
 */
QString TableSpinBoxDelegate::displayText(const QVariant& value,
                                          const QLocale& locale) const {
  if (value.type() == QVariant::Double) { // frame and return a formatted
                                          // QString if handling a cell with
                                          // double value
    return locale.toString((value.toDouble()), 'f', _doubleSpinBoxPrecision);
  } else { // else let the default delegate take care of formatting if
           // content is not double
    return QStyledItemDelegate::displayText(value, locale);
  }
}

/*
 * Use a customized QDoubleSpinbox to modify cells with double value. The
 * spinbox supports a precision specified by _doubleSpinBoxPrecision.
 */
QWidget* TableSpinBoxDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const {
  // TODO: Modify to make created editor 'column independent' - All
  // cells accepting double in the table should
  // have a common type of editor.
  // <chk source: QStyledItemDelegate::createEditor>
  //
  if (index.data(Qt::EditRole).type() == QVariant::Double) { // create spinbox with custom precision
                                       // for cells in the double column
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(parent);
    doubleSpinBox->setDecimals(_doubleSpinBoxPrecision);
    doubleSpinBox->setMaximum(MAX_VALUE);
    doubleSpinBox->setMinimum(MIN_VALUE);
    return doubleSpinBox;
  } else {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

void TableSpinBoxDelegate::setDoubleSpinBoxPrecision(
    unsigned int doubleSpinBoxPrecision) {
  _doubleSpinBoxPrecision = static_cast<int>(doubleSpinBoxPrecision);
}

