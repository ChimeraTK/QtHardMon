/*
 * TableSpinBoxDelegate.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: varghese
 */

#include "TableSpinBoxDelegate.h"
#include <QDoubleSpinBox>
#include <QStringBuilder>
#include "HexSpinBox.h"

static const int DOUBLE_SPINBOX_MAX_VALUE = 1000000000;
static const int DOUBLE_SPINBOX_MIN_VALUE = -1000000000;

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
  } else if(value.type() == QVariant::UserType){
    Mytype a = value.value<Mytype>();
    int value = a.i;
    return (QString("0x") % QString::number(value, 16));
    }
  else { // else let the default delegate take care of formatting if
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
  if (index.column() ==
      FLOATING_POINT_DISPLAY_COLUMN) { // create spinbox with custom precision
                                       // for cells in the double column
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(parent);
    doubleSpinBox->setDecimals(_doubleSpinBoxPrecision);
    doubleSpinBox->setMaximum(DOUBLE_SPINBOX_MAX_VALUE);
    doubleSpinBox->setMinimum(DOUBLE_SPINBOX_MIN_VALUE);
    return doubleSpinBox;
  } else if(index.data().type() == QVariant::UserType){

    HexSpinBox* hexSpinBox = new HexSpinBox(parent);
    hexSpinBox->setMaximum(DOUBLE_SPINBOX_MAX_VALUE);
    hexSpinBox->setMinimum(DOUBLE_SPINBOX_MIN_VALUE);
    return hexSpinBox;
  }
  else {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

void TableSpinBoxDelegate::setDoubleSpinBoxPrecision(
    unsigned int doubleSpinBoxPrecision) {
  _doubleSpinBoxPrecision = static_cast<int>(doubleSpinBoxPrecision);
}

/*
 * Use a customized QDoubleSpinbox to modify cells with double value. The
 * spinbox supports a precision specified by _doubleSpinBoxPrecision.
 */


void
TableSpinBoxDelegate::setEditorData (QWidget* editor,
				     const QModelIndex& index) const {
  if (index.data(Qt::EditRole).type() == QVariant::UserType
      ) {
    Mytype value = index.data(Qt::EditRole).value<Mytype>();
    HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
    spinBox->setValue(value.i);
  } else{
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

void
TableSpinBoxDelegate::setModelData (QWidget* editor, QAbstractItemModel* model,
				    const QModelIndex& index) const {
  if (index.data(Qt::EditRole).type() == QVariant::UserType) {
    HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();
    Mytype a;
    a.i = value;
    QVariant test;
    test.setValue(a);
    model->setData(index, test, Qt::EditRole);

  } else {
    QStyledItemDelegate::setModelData(editor, model, index);
  }

}
