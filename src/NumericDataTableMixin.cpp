#include "NumericDataTableMixin.h"
#include "Exceptions.h"
#include "Constants.h"
#include "PreferencesProvider.h"

NumericDataTableMixin::NumericDataTableMixin() : 
defaultBackgroundBrush_(Qt::transparent),
modifiedBackgroundBrush_(QColor(255, 100, 100, 255))
{
    PreferencesProvider & preferencesProvider = PreferencesProviderSingleton::Instance();
    int floatPrecision;
    try {
        floatPrecision = preferencesProvider.getValue<int>("floatPrecision");
    } catch(InvalidOperationException & e) {
        floatPrecision = static_cast<int>(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION);
        preferencesProvider.setValue("floatPrecision", floatPrecision);
    }
    customDelegate_.setDoubleSpinBoxPrecision(floatPrecision);
}

void NumericDataTableMixin::setTableWidget(QTableWidget * widget) {
    tableWidget_ = widget;
    tableWidget_->setItemDelegate(&customDelegate_);
}

QTableWidget * NumericDataTableMixin::getTableWidget() {
    if (!tableWidget_) {
        throw InternalErrorException (
            "DataTableWidget was not set properly. Report a bug to developers."
        );
    } else
        return tableWidget_;
}

void NumericDataTableMixin::updateTableEntries(int row, int column) {
    PreferencesProvider & preferencesProvider = PreferencesProviderSingleton::Instance();
    customDelegate_.setDoubleSpinBoxPrecision(preferencesProvider.getValue<int>("floatPrecision"));
  // We have two editable fields - The decimal field and double field.
  // The values reflect each other and to avoid an infinite
  // loop  situation,  corresponding column cells are updated
  // only if required
  //
  if (column == qthardmon::FIXED_POINT_DISPLAY_COLUMN) {
    HexData hexValue;
    int userUpdatedValueInCell = readCell<int>(row, column);
    hexValue.value = userUpdatedValueInCell;
    double fractionalVersionOfUserValue =
        convertToDouble(userUpdatedValueInCell);

    // update the hex field in all  cases
    writeCell<HexData>(row, qthardmon::HEX_VALUE_DISPLAY_COLUMN, hexValue);

    if (isValidCell(row, qthardmon::FLOATING_POINT_DISPLAY_COLUMN)) {
      double currentValueInDoubleColumn =
          readCell<double>(row, qthardmon::FLOATING_POINT_DISPLAY_COLUMN);
      if (currentValueInDoubleColumn == fractionalVersionOfUserValue)
        return; // same value in the corresponding double cell, so not updating
                // this cell
    }
    // If here, This is a new value. Trigger update of the float cell

    writeCell<double>(row, qthardmon::FLOATING_POINT_DISPLAY_COLUMN,
                      fractionalVersionOfUserValue);

  } else if (column == qthardmon::FLOATING_POINT_DISPLAY_COLUMN) {
    double userUpdatedValueInCell = readCell<double>(row, column);
    int FixedPointVersionOfUserValue =
        convertToFixedPoint(userUpdatedValueInCell);

    if (isValidCell(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN)) {
      int currentValueInFixedPointCell =
          readCell<int>(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN);
      double convertedValueFrmFPCell =
          convertToDouble(currentValueInFixedPointCell);
      if (userUpdatedValueInCell == convertedValueFrmFPCell)
        return;
    }

    writeCell<int>(
        row, qthardmon::FIXED_POINT_DISPLAY_COLUMN,
        FixedPointVersionOfUserValue); // This will trigger an update to
                                       // the fixed point display column,
                                       // which will in turn correct the
                                       // value in this double cell to a
    // valid one (In case the user entered one is not supported by the floating
    // point converter settings)
  } else if (column == qthardmon::HEX_VALUE_DISPLAY_COLUMN) {
    HexData hexInCell = readCell<HexData>(row, column);
    int userUpdatedValueInCell = hexInCell.value;

    if (isValidCell(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN)) {
      int currentValueInFixedPointCell =
          readCell<int>(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN);
      if (userUpdatedValueInCell == currentValueInFixedPointCell)
        return;
    }
    writeCell<int>(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN, userUpdatedValueInCell);
  }

  //   if (insideReadOrWrite_==0){
//     getTableWidget()->item(row, column)->setBackground( modifiedBackgroundBrush_ );
//   }
//   else{
//     clearRowBackgroundColour(row);
//   }
}


int NumericDataTableMixin::getNumberOfColumsInTableWidget() {
  return (getTableWidget()->columnCount());
}


bool NumericDataTableMixin::isValidCell(int row, int columnIndex) {
  return (getTableWidget()->item(row, columnIndex) != NULL);
}

void NumericDataTableMixin::clearCellBackground(int row, int columnIndex) {
  getTableWidget()->item(row, columnIndex)
      ->setBackground(defaultBackgroundBrush_);
}

double NumericDataTableMixin::convertToDouble(int decimalValue) {
    if (converter_)
        return converter_->toDouble(decimalValue);
    else 
        return (double) decimalValue;
}

int NumericDataTableMixin::convertToFixedPoint(double doubleValue) {
    if (converter_)
        return converter_->toFixedPoint(doubleValue);
    else 
        return (int) doubleValue;
}

void NumericDataTableMixin::setFixedPointConverter(mtca4u::FixedPointConverter * converter) {
    converter_ = converter;
}

void NumericDataTableMixin::clearAllRowsInTable() {
  getTableWidget()->clearContents();
  int nRows = 0;
  getTableWidget()->setRowCount(nRows);
}

void NumericDataTableMixin::clearRowBackgroundColour(int row) {
  int numberOfColumns = getNumberOfColumsInTableWidget();
  for (int columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
    if (isValidCell(row, columnIndex)) {
      clearCellBackground(row, columnIndex);
    }
  }
}

void NumericDataTableMixin::addCopyActionForTableWidget() {
//   QAction *copy = new QAction("&Copy", getTableWidget());
//   copy->setShortcuts(QKeySequence::Copy);
//   copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
//   connect(copy, SIGNAL(triggered()), this, SLOT(copyTableDataToClipBoard()));
//   getTableWidget()->addAction(copy);
}

void NumericDataTableMixin::clearBackground(){
  int nRows = getTableWidget()->rowCount();

  for( int row=0; row < nRows; ++row ){
    clearRowBackgroundColour(row);
  }
}