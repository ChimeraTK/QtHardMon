/*
 * TableSpinBoxDelegate.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: varghese
 */

#include "TableSpinBoxDelegate.h"
#include <QDoubleSpinBox>
#include "HexSpinBox.h"
#include "qbytearray.h"
#include <iostream>


static const int DOUBLE_SPINBOX_MAX_VALUE = 1000000000;
static const int DOUBLE_SPINBOX_MIN_VALUE = -1000000000;


enum myEnum{
  a = 259
};


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

  int dataTypeInCell = value.type();

  if (value.type() == QVariant::Double) { // frame and return a formatted
                                          // QString if handling a cell with
                                          // double value
    return locale.toString((value.toDouble()), 'f', _doubleSpinBoxPrecision);
  } else if(false){//value.type() == QVariant::UserType){
    Mytype a = value.value<Mytype>();
    double storedValue = a.i;
    return locale.toString((a.toDouble()), 'f', _doubleSpinBoxPrecision);
  }else { // else let the default delegate take care of formatting if
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
 int dataTypeInCell = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());

  if (dataTypeInCell ==
      259) { // create spinbox with custom precision
                                       // for cells in the double column
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(parent);
    //HexSpinBox* doubleSpinBox = new HexSpinBox(parent);
    //doubleSpinBox->setDecimals(_doubleSpinBoxPrecision);
    doubleSpinBox->setMaximum(DOUBLE_SPINBOX_MAX_VALUE);
    doubleSpinBox->setMinimum(DOUBLE_SPINBOX_MIN_VALUE);
    return doubleSpinBox;
  } else {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

void TableSpinBoxDelegate::setDoubleSpinBoxPrecision(
    unsigned int doubleSpinBoxPrecision) {
  _doubleSpinBoxPrecision = static_cast<int>(doubleSpinBoxPrecision);
}

void
TableSpinBoxDelegate::setEditorData (QWidget* editor,
				     const QModelIndex& index) const {
/*  int dataTypeInCell = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());

  if (dataTypeInCell == 259
      ) {
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
  } else*/ {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

void
TableSpinBoxDelegate::setModelData (QWidget* editor, QAbstractItemModel* model,
				    const QModelIndex& index) const {
/*  int dataTypeInCell = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());
  if (dataTypeInCell ==
      259) {
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    Mytype a;
    a.i = value;
    QVariant test;
    test.setValue(a);
    model->setData(index, test, Qt::EditRole);

  } else*/ {
    QStyledItemDelegate::setModelData(editor, model, index);
  }

}
