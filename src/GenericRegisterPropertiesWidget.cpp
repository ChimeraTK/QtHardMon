#include "GenericRegisterPropertiesWidget.h"
#include "Constants.h"


// The default maximum for the number of words in a register.
// This limits the number of rows in the valuesTableWidget to avoid a segmentation fault if too much
// memory is requested.
static const size_t DEFAULT_MAX_WORDS = 0x10000;

GenericRegisterPropertiesWidget::GenericRegisterPropertiesWidget(QWidget *parent) :
    PropertiesWidget(parent),
    ui(new Ui::GenericRegisterPropertiesWidget),
    defaultBackgroundBrush_( Qt::transparent ),
    modifiedBackgroundBrush_( QColor( 255, 100, 100, 255 ) ), // red, not too dark
    floatPrecision_(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION),
     maxWords_( DEFAULT_MAX_WORDS )
{
    ui->setupUi(this);

    clearFields();

    connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)),
    this, SLOT( updateTableEntries(int, int) ) );
      connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)), 
	  this, SLOT( changeBackgroundIfModified(int, int) ) );
  customDelegate_.setDoubleSpinBoxPrecision(floatPrecision_);
    ui->valuesTableWidget->setItemDelegate(&customDelegate_);

    addCopyActionForTableWidget(); // Ctrl + c dosent do anything for now

}

GenericRegisterPropertiesWidget::~GenericRegisterPropertiesWidget()
{
    delete ui;
}

void GenericRegisterPropertiesWidget::clearFields() {
    ui->registerNameDisplay     ->setText("");
    ui->moduleDisplay           ->setText("");
    ui->registerNElementsDisplay->setText("");
}

void GenericRegisterPropertiesWidget::updateTableEntries(int row, int column) {

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
}

int GenericRegisterPropertiesWidget::getNumberOfColumsInTableWidget() {
  return (ui->valuesTableWidget->columnCount());
}

bool GenericRegisterPropertiesWidget::isValidCell(int row, int columnIndex) {
  return (ui->valuesTableWidget->item(row, columnIndex) != NULL);
}

void GenericRegisterPropertiesWidget::clearCellBackground(int row, int columnIndex) {
  ui->valuesTableWidget->item(row, columnIndex)
      ->setBackground(defaultBackgroundBrush_);
}

double GenericRegisterPropertiesWidget::convertToDouble(int decimalValue) {
    return (double) decimalValue;
}

int GenericRegisterPropertiesWidget::convertToFixedPoint(double doubleValue) {
    return (int) doubleValue;
}

void GenericRegisterPropertiesWidget::clearAllRowsInTable() {
  ui->valuesTableWidget->clearContents();
  int nRows = 0;
  ui->valuesTableWidget->setRowCount(nRows);
}

void GenericRegisterPropertiesWidget::clearRowBackgroundColour(int row) {
  int numberOfColumns = getNumberOfColumsInTableWidget();
  for (int columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
    if (isValidCell(row, columnIndex)) {
      clearCellBackground(row, columnIndex);
    }
  }
}

void GenericRegisterPropertiesWidget::addCopyActionForTableWidget() {
  QAction *copy = new QAction(tr("&Copy"), ui->valuesTableWidget);
  copy->setShortcuts(QKeySequence::Copy);
  copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(copy, SIGNAL(triggered()), this, SLOT(copyTableDataToClipBoard()));
  ui->valuesTableWidget->addAction(copy);
}

void GenericRegisterPropertiesWidget::clearBackground(){
  int nRows = ui->valuesTableWidget->rowCount();

  for( int row=0; row < nRows; ++row ){
    clearRowBackgroundColour(row);
  }
}


void GenericRegisterPropertiesWidget::changeBackgroundIfModified( int row, int column ){
//   if (insideReadOrWrite_==0){
//     ui->valuesTableWidget->item(row, column)->setBackground( modifiedBackgroundBrush_ );
//   }
//   else{
//     clearRowBackgroundColour(row);
//   }
}


void GenericRegisterPropertiesWidget::copyTableDataToClipBoard(){
	//TODO: SOmething for later. Not Implemented yet
}


void GenericRegisterPropertiesWidget::setSize(int nOfElements, int size) {
    ui->registerNElementsDisplay->setText(QString::number(nOfElements));
}

void GenericRegisterPropertiesWidget::setNames(std::vector<std::string> components) {
    std::string moduleName;
    if (components.size() >= 2) {
        for (int i = 0; i < components.size() - 1; ++i) {
            moduleName += "/" + components[i];
        }
        moduleName = moduleName.substr(1);
    } else 
        moduleName = "";

    ui->registerNameDisplay->setText(components.back().c_str());
    ui->moduleDisplay->setText(moduleName.c_str());
}

void GenericRegisterPropertiesWidget::setFixedPointInfo(int width, int fracBits, int signBit) {
}

void GenericRegisterPropertiesWidget::setAddress(int bar, int address) {
}