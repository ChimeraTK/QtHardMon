#ifndef GENERICREGISTERPROPERTIESWIDGET_H
#define GENERICREGISTERPROPERTIESWIDGET_H

#include "PropertiesWidget.h"
#include "ui_GenericRegisterPropertiesWidget.h"
#include "CustomDelegates.h"

namespace Ui {
class GenericRegisterPropertiesWidget;
}

class GenericRegisterPropertiesWidget : public PropertiesWidget {
    Q_OBJECT

public:

public:
    explicit GenericRegisterPropertiesWidget(QWidget *parent);
    ~GenericRegisterPropertiesWidget();
    
    virtual void clearFields();
    virtual void setSize(int nOfElements, int nOfChannels);
    virtual void setNames(std::vector<std::string> components);
    virtual void setFixedPointInfo(int width, int fracBits, int signBit);
    virtual void setAddress(int bar, int address);


  double convertToDouble(int decimalValue);

  int convertToFixedPoint(double doubleValue);

private slots:
    void updateTableEntries( int row, int column );
      void changeBackgroundIfModified( int row, int column );
      void copyTableDataToClipBoard();
private:
    int getNumberOfColumsInTableWidget();
    bool isValidCell(int row, int columnIndex);
    void clearCellBackground(int row, int columnIndex);
    template<typename T>
    void writeCell(int row, int column, T value);
    template<typename T>
    T readCell (int row, int column);
  void clearAllRowsInTable();
  void clearRowBackgroundColour(int row);
  void addCopyActionForTableWidget();
    void clearBackground();


public:
    Ui::GenericRegisterPropertiesWidget *ui;
    CustomDelegates customDelegate_;
    QBrush defaultBackgroundBrush_;
    QBrush modifiedBackgroundBrush_;
    unsigned int maxWords_;
    unsigned int floatPrecision_;

    friend class QtHardMon;
    friend class PlotWindow;
};

template <typename T> void GenericRegisterPropertiesWidget::writeCell(int row, int column, T value) {
  QTableWidgetItem *widgetItem = new QTableWidgetItem();
  QVariant dataVariant;
  dataVariant.setValue(value);
  widgetItem->setData(Qt::DisplayRole, dataVariant);
  ui->valuesTableWidget->setItem(row, column, widgetItem);
}

template <typename T> T GenericRegisterPropertiesWidget::readCell(int row, int column) {
  return (ui->valuesTableWidget->item(row, column)
              ->data(Qt::DisplayRole)
              .value<T>());
}



#endif // GENERICREGISTERPROPERTIESWIDGET_H