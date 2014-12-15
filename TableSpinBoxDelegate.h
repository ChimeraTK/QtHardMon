/*
 * TableSpinBoxDelegate.h
 *
 *  Created on: Dec 15, 2014
 *      Author: varghese
 */

#ifndef TABLESPINBOXDELEGATE_H_
#define TABLESPINBOXDELEGATE_H_

#include <QStyledItemDelegate>

class TableSpinBoxDelegate: public QStyledItemDelegate {
  Q_OBJECT
public:
  TableSpinBoxDelegate ();
  void setDoublePrecision(int doubleSpinBoxPrecision);
  QWidget *createEditor(QWidget *parent,
      const QStyleOptionViewItem & option,
      const QModelIndex & index ) const;
  virtual
  ~TableSpinBoxDelegate ();
private:
int _doubleSpinBoxPrecision;
int _maximumDoubleVaue;
};

#endif /* TABLESPINBOXDELEGATE_H_ */
