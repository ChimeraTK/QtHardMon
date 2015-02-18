/*
 * TableSpinBoxDelegate.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: varghese
 */

#include "TableSpinBoxDelegate.h"
#include <QDoubleSpinBox>
#include <QStringBuilder>



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
    // The hex type is the only defined type at this point so this check should be OK

    HexData hexValue = value.value<HexData>();
    int intValue = hexValue.value;
    return (QString("0x") % QString::number(intValue, 16));

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

  if (index.data(Qt::EditRole).type() == QVariant::Double) { // create spinbox with custom precision
                                       // for cells in the double column
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(parent);
    doubleSpinBox->setDecimals(_doubleSpinBoxPrecision);
    doubleSpinBox->setRange(MIN_VALUE, MAX_VALUE);
    return doubleSpinBox;
  } else if (index.data(Qt::EditRole).type() == QVariant::Int) {
    // Want to customize the decimal spin boxes to limit the max and minimum value they can display
    QSpinBox* decimalSpinbox = new QSpinBox(parent);
    decimalSpinbox->setRange(MIN_VALUE, MAX_VALUE);
    return decimalSpinbox;
  } else if(index.data(Qt::EditRole).type() == QVariant::UserType){
    // TODO: make the check above specific to our custom HexDataType
    HexSpinBox* hexSpinBox = new HexSpinBox(parent);
    hexSpinBox->setRange(MIN_VALUE, MAX_VALUE);
    return hexSpinBox;
  }
  else {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
}


void
TableSpinBoxDelegate::setEditorData (QWidget* editor,
				     const QModelIndex& index) const {
  if (index.data(Qt::EditRole).type() == QVariant::UserType
      ) {
    HexData hexData = index.data(Qt::EditRole).value<HexData>();
    HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
    spinBox->setValue(hexData.value);
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
    HexData hexData;
    hexData.value = value;
    QVariant dataVariant;
    dataVariant.setValue(hexData);
    model->setData(index, dataVariant, Qt::EditRole);

  } else {
    QStyledItemDelegate::setModelData(editor, model, index);
  }

}

void TableSpinBoxDelegate::setDoubleSpinBoxPrecision(
    unsigned int doubleSpinBoxPrecision) {
  _doubleSpinBoxPrecision = static_cast<int>(doubleSpinBoxPrecision);
}

