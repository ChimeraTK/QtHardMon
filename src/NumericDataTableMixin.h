#ifndef NUMERICDATATABLEMIXIN_H
#define NUMERICDATATABLEMIXIN_H

#include "CustomDelegates.h"
#include <QtGui/QTableWidget>
#include <mtca4u/FixedPointConverter.h>

class NumericDataTableMixin {

public:
  NumericDataTableMixin();
  int getNumberOfColumsInTableWidget();
  bool isValidCell(int row, int columnIndex);
  void clearCellBackground(int row, int columnIndex);
  template <typename T> void writeCell(int row, int column, T value);
  template <typename T> T readCell(int row, int column);
  void clearAllRowsInTable();
  void clearRowBackgroundColour(int row);
  void addCopyActionForTableWidget();
  void clearBackground();
  double convertToDouble(int decimalValue);
  void setFixedPointConverter(mtca4u::FixedPointConverter *converter);
  int convertToFixedPoint(double doubleValue);
  void setTableWidget(QTableWidget *widget);
  void updateTableEntries(int row, int column);

  virtual QTableWidget *getTableWidget();

public:
  CustomDelegates customDelegate_;
  QBrush defaultBackgroundBrush_;
  QBrush modifiedBackgroundBrush_;
  mtca4u::FixedPointConverter *converter_;

  QTableWidget *tableWidget_;
};

template <typename T>
void NumericDataTableMixin::writeCell(int row, int column, T value) {
  QTableWidgetItem *widgetItem = new QTableWidgetItem();
  QVariant dataVariant;
  dataVariant.setValue(value);
  widgetItem->setData(Qt::DisplayRole, dataVariant);
  getTableWidget()->setItem(row, column, widgetItem);
}

template <typename T> T NumericDataTableMixin::readCell(int row, int column) {
  return (
      getTableWidget()->item(row, column)->data(Qt::DisplayRole).value<T>());
}

#endif // NUMERICDATATABLEMIXIN_H