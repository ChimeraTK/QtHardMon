#ifndef REGISTERPROPERTIESWIDGET_H
#define REGISTERPROPERTIESWIDGET_H

#include <QWidget>
#include "ui_RegisterPropertiesWidget.h"
#include "CustomDelegates.h"

#include <mtca4u/FixedPointConverter.h>
namespace Ui {
class RegisterPropertiesWidget;
}

class RegisterPropertiesWidget : public QWidget
{
    Q_OBJECT

public:
    struct RegisterProperties {

    QString registerName;
    QString moduleName;
    QString bar;
    QString address;
    QString nOfElements;
    QString size;
    QString width;
    QString fracBits;
    QString signBit;

    RegisterProperties(QString RegisterName = "",
QString ModuleName = "",
QString Bar = "",
QString Address = "",
QString NOfElements = "",
QString Size = "",
QString Width = "",
QString FracBits = "",
QString SignBit = "");
    };

public:
    explicit RegisterPropertiesWidget(QWidget *parent);
    ~RegisterPropertiesWidget();
    
    void clearProperties();
    void setRegisterProperties(RegisterProperties properties);


  double convertToDouble(int decimalValue);

  void setFixedPointConverter(mtca4u::FixedPointConverter * converter);

  int convertToFixedPoint(double doubleValue);

private slots:
    void updateTableEntries( int row, int column );
private:
    int getNumberOfColumsInTableWidget();
    bool isValidCell(int row, int columnIndex);
    void clearCellBackground(int row, int columnIndex);
    template<typename T>
    void writeCell(int row, int column, T value);
    template<typename T>
    T readCell (int row, int column);

public:
    Ui::RegisterPropertiesWidget *ui;
    CustomDelegates customDelegate_;
    QBrush defaultBackgroundBrush_;
    mtca4u::FixedPointConverter * converter_;


    friend class QtHardMon;
    friend class PlotWindow;
};

template <typename T> void RegisterPropertiesWidget::writeCell(int row, int column, T value) {
  QTableWidgetItem *widgetItem = new QTableWidgetItem();
  QVariant dataVariant;
  dataVariant.setValue(value);
  widgetItem->setData(Qt::DisplayRole, dataVariant);
  ui->valuesTableWidget->setItem(row, column, widgetItem);
}

template <typename T> T RegisterPropertiesWidget::readCell(int row, int column) {
  return (ui->valuesTableWidget->item(row, column)
              ->data(Qt::DisplayRole)
              .value<T>());
}
#endif // REGISTERPROPERTIESWIDGET_H
