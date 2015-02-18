/*
 * TableSpinBoxDelegate.h
 *
 *  Created on: Dec 15, 2014
 *      Author: varghese
 */

#ifndef TABLESPINBOXDELEGATE_H_
#define TABLESPINBOXDELEGATE_H_

#include <QStyledItemDelegate>
#include <QObject>
#include "HexSpinBox.h"



/**
 * TableSpinBoxDelegate provides  custom implemented delegates
 * that can be used to modify cell display properties of a QTableWidget. A
 * QTableWidget linked with a TableSpinBoxDelegate object will use the delegates
 * of this class instead of the base class(QStyledItemDelegate)
 */
class TableSpinBoxDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  // Constants
  static const unsigned int DOUBLE_SPINBOX_DEFAULT_PRECISION = 4;
  static const int MAX_VALUE = 1000000000;
  static const int MIN_VALUE = -1000000000;

  /**
   * Default constructor
   */
  TableSpinBoxDelegate(QObject *parent = 0);

  /**
   * This method is used to specify the number of decimal places that
   * should be displayed for a float value in the FLOATING_POINT_DISPLAY_COLUMN
   * of the table widget
   */
  void setDoubleSpinBoxPrecision(unsigned int doubleSpinBoxPrecision);

  /**
   *  This delegate is responsible for the format and content displayed in a
   *  cell. Overriding the base class version of this method enables
   *  customization of the displayed data format.
   */
  QString displayText(const QVariant &value, const QLocale &locale) const;

  /**
   * This delegate is called when a cell is double clicked/selected  to
   * modify its value. Overriding this delegate lets us customize the
   * type of editor widget displayed in the cell when clicked for editing
   */
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;


  void
  setEditorData (QWidget* editor,
  				     const QModelIndex& index) const;

  void
  setModelData (QWidget* editor, QAbstractItemModel* model,
  				    const QModelIndex& index) const;

private:
  int _doubleSpinBoxPrecision;
};

#endif /* TABLESPINBOXDELEGATE_H_ */
