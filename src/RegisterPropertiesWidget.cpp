#include "RegisterPropertiesWidget.h"
#include "Constants.h"


// The default maximum for the number of words in a register.
// This limits the number of rows in the valuesTableWidget to avoid a segmentation fault if too much
// memory is requested.
static const size_t DEFAULT_MAX_WORDS = 0x10000;

RegisterPropertiesWidget::RegisterPropertiesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPropertiesWidget),
    defaultBackgroundBrush_( Qt::transparent ),
    modifiedBackgroundBrush_( QColor( 255, 100, 100, 255 ) ), // red, not too dark
    floatPrecision_(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION),
     maxWords_( DEFAULT_MAX_WORDS )
{
    ui->setupUi(this);

    clearProperties();

    connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)),
    this, SLOT( updateTableEntries(int, int) ) );
      connect(ui->valuesTableWidget, SIGNAL(cellChanged(int, int)), 
	  this, SLOT( changeBackgroundIfModified(int, int) ) );
  customDelegate_.setDoubleSpinBoxPrecision(floatPrecision_);
    ui->valuesTableWidget->setItemDelegate(&customDelegate_);

    addCopyActionForTableWidget(); // Ctrl + c dosent do anything for now

}

RegisterPropertiesWidget::~RegisterPropertiesWidget()
{
    delete ui;
}

void RegisterPropertiesWidget::clearProperties() {
    RegisterProperties properties;
    setRegisterProperties(properties);
}

void RegisterPropertiesWidget::setRegisterProperties(RegisterProperties properties) {
    ui->registerNameDisplay     ->setText(properties.registerName);
    ui->moduleDisplay           ->setText(properties.moduleName);
    ui->registerBarDisplay      ->setText(properties.bar);
    ui->registerNElementsDisplay->setText(properties.nOfElements);
    ui->registerAddressDisplay  ->setText(properties.address);
    ui->registerSizeDisplay     ->setText(properties.size);
    ui->registerWidthDisplay    ->setText(properties.width);
    ui->registerFracBitsDisplay ->setText(properties.fracBits);
    ui->registeSignBitDisplay   ->setText(properties.signBit);
}

void RegisterPropertiesWidget::updateTableEntries(int row, int column) {

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

RegisterPropertiesWidget::RegisterProperties::RegisterProperties(
QString RegisterName,
QString ModuleName,
QString Bar,
QString Address,
QString NOfElements,
QString Size,
QString Width,
QString FracBits,
QString SignBit
) 
        : registerName(RegisterName),
            moduleName(ModuleName),
            bar(Bar),
            address(Address),
            nOfElements(NOfElements),
            size(Size),
            width(Width),
            fracBits(FracBits),
            signBit(SignBit) {}

int RegisterPropertiesWidget::getNumberOfColumsInTableWidget() {
  return (ui->valuesTableWidget->columnCount());
}

bool RegisterPropertiesWidget::isValidCell(int row, int columnIndex) {
  return (ui->valuesTableWidget->item(row, columnIndex) != NULL);
}

void RegisterPropertiesWidget::clearCellBackground(int row, int columnIndex) {
  ui->valuesTableWidget->item(row, columnIndex)
      ->setBackground(defaultBackgroundBrush_);
}

double RegisterPropertiesWidget::convertToDouble(int decimalValue) {
    if (converter_)
        return converter_->toDouble(decimalValue);
    else 
        return (double) decimalValue;
}

int RegisterPropertiesWidget::convertToFixedPoint(double doubleValue) {
    if (converter_)
        return converter_->toFixedPoint(doubleValue);
    else 
        return (int) doubleValue;
}

void RegisterPropertiesWidget::setFixedPointConverter(mtca4u::FixedPointConverter * converter) {
    converter_ = converter;
}

void RegisterPropertiesWidget::clearAllRowsInTable() {
  ui->valuesTableWidget->clearContents();
  int nRows = 0;
  ui->valuesTableWidget->setRowCount(nRows);
}

void RegisterPropertiesWidget::clearRowBackgroundColour(int row) {
  int numberOfColumns = getNumberOfColumsInTableWidget();
  for (int columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
    if (isValidCell(row, columnIndex)) {
      clearCellBackground(row, columnIndex);
    }
  }
}

void RegisterPropertiesWidget::addCopyActionForTableWidget() {
  QAction *copy = new QAction(tr("&Copy"), ui->valuesTableWidget);
  copy->setShortcuts(QKeySequence::Copy);
  copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(copy, SIGNAL(triggered()), this, SLOT(copyTableDataToClipBoard()));
  ui->valuesTableWidget->addAction(copy);
}

void RegisterPropertiesWidget::clearBackground(){
  int nRows = ui->valuesTableWidget->rowCount();

  for( int row=0; row < nRows; ++row ){
    clearRowBackgroundColour(row);
  }
}


void RegisterPropertiesWidget::changeBackgroundIfModified( int row, int column ){
//   if (_insideReadOrWrite==0){
//     ui->valuesTableWidget->item(row, column)->setBackground( modifiedBackgroundBrush_ );
//   }
//   else{
//     clearRowBackgroundColour(row);
//   }
}


void RegisterPropertiesWidget::copyTableDataToClipBoard(){
	//TODO: SOmething for later. Not Implemented yet
}