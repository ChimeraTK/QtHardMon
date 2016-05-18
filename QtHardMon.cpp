#include "QtHardMon.h"
#include "QtHardMonVersion.h"
#include "ui_PreferencesForm.h"
#include "ConfigFileReaderWriter.h"

#include <iostream>
#include <limits>
#include <sstream>

#include <QMessageBox>
#include <QFileDialog>
#include <QDockWidget>
#include <qaction.h>

#include <mtca4u/Exception.h>
#include <mtca4u/DMapFilesParser.h>
#include <mtca4u/NumericAddressedBackendMuxedRegisterAccessor.h>
#include <mtca4u/PcieBackendException.h>
#include <mtca4u/BackendFactory.h>

#include <QTextStream>
#include <QDebug>
#include "Constants.h"
#include "Exceptions.h"
using namespace mtca4u;



// The default maximum for the number of words in a register.
// This limits the number of rows in the valuesTableWidget to avoid a segmentation fault if too much
// memory is requested.
static const size_t DEFAULT_MAX_WORDS = 0x10000;

// Some variables to avoid duplication and possible inconsistencies in the code.
// These strings are used in the config file
#define DMAP_FILE_STRING "dmapFile"
#define CURRENT_DEVICE_STRING "currentDevice"
#define CURRENT_REGISTER_STRING "currentRegister"
#define CURRENT_MODULE_STRING "currentModule"
#define MAX_WORDS_STRING "maxWords"
#define PRECISION_INDICATOR_STRING "decimalPlaces"
#define READ_AFTER_WRITE_STRING "readAfterWrite"
#define AUTOSELECT_PREVIOUS_REGISTER_STRING "autoselectPreviousRegister"
#define SHOW_PLOT_WINDOW_STRING "showPlotWindow"
#define PLOT_AFTER_READ_STRING "plotAfterRead"
#define REGISTER_EXTENSION_STRING "_REGISTER"
#define MODULE_EXTENSION_STRING "_MODULE"
#define FONT_SIZE_STRING "fontSize"
#define AUTO_READ_STRING "autoRead"
#define READ_ON_CLICK_STRING "readOnClick"
#define NO_MODULE_NAME_STRING "[No Module Name]"

QtHardMon::QtHardMon(QWidget * parent_, Qt::WindowFlags flags) 
  : QMainWindow(parent_, flags),_hardMonForm(),  _mtcaDevice(), _maxWords( DEFAULT_MAX_WORDS ),
    _floatPrecision(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION),_autoRead(true),
    _readOnClick(true), _dmapFileName(), _configFileName(), _insideReadOrWrite(0),
    _defaultBackgroundBrush( Qt::transparent ), // transparent
    _modifiedBackgroundBrush( QColor( 255, 100, 100, 255 ) ), // red, not too dark
    _customDelegate(),
    _currentDeviceListItem(NULL),
    _plotWindow(NULL)
{



  _hardMonForm.setupUi(this);

  setWindowTitle("QtHardMon");
  setWindowIcon(  QIcon(":/DESY_logo_nofade.png") );
  _hardMonForm.logoLabel->setPixmap( QPixmap(":/DESY_logo.png") );

  // This brings in support for Ctrl + c for copying data to the clipboard.
  addCopyActionForTableWidget(); // Ctrl + c dosent do anything for now
  addCopyActionForRegisterTreeWidget(); // Adds slot to copy qtreeiem's name to
                                        // clipboard

  connect(_hardMonForm.deviceListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
	  this, SLOT( deviceSelected(QListWidgetItem *, QListWidgetItem *) ) );

  connect(_hardMonForm.registerTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), 
	  this, SLOT( registerSelected(QTreeWidgetItem *, QTreeWidgetItem *) ) );

  connect(_hardMonForm.registerTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)), 
	  this, SLOT( registerClicked(QTreeWidgetItem *) ) );

  connect(_hardMonForm.valuesTableWidget, SIGNAL(cellChanged(int, int)),
	  this, SLOT( updateTableEntries(int, int) ) );

  connect(_hardMonForm.valuesTableWidget, SIGNAL(cellChanged(int, int)), 
	  this, SLOT( changeBackgroundIfModified(int, int) ) );

  connect(_hardMonForm.loadBoardsButton, SIGNAL(clicked()),
	  this, SLOT(loadBoards()));
					       
  connect(_hardMonForm.readButton, SIGNAL(clicked()),
	  this, SLOT(read()));

  connect(_hardMonForm.writeButton, SIGNAL(clicked()),
	  this, SLOT(write()));

  connect(_hardMonForm.aboutQtHardMonAction, SIGNAL(triggered()),
	  this, SLOT(aboutQtHardMon()));

  connect(_hardMonForm.aboutQtAction, SIGNAL(triggered()),
	  this, SLOT(aboutQt()));

  connect(_hardMonForm.preferencesAction, SIGNAL(triggered()),
	  this, SLOT(preferences()));

  connect(_hardMonForm.loadConfigAction, SIGNAL(triggered()),
	  this, SLOT(loadConfig()));

  connect(_hardMonForm.saveConfigAction, SIGNAL(triggered()),
	  this, SLOT(saveConfig()));

  connect(_hardMonForm.saveConfigAsAction, SIGNAL(triggered()),
	  this, SLOT(saveConfigAs()));

  connect(_hardMonForm.loadBoardsAction, SIGNAL(triggered()),
	  this, SLOT(loadBoards()));

  connect(_hardMonForm.quitAction, SIGNAL(triggered()),
	  this, SLOT(close()));

  connect(_hardMonForm.openCloseButton, SIGNAL(clicked()),
	  this, SLOT(openCloseDevice()));

  connect(_hardMonForm.SortAscendingcheckBox, SIGNAL(stateChanged(int)),
	  this, SLOT(handleSortCheckboxClick(int)));

  // The oparations and options group are disabled until a dmap file is loaded and a device has been opened 
  _hardMonForm.operationsGroupBox->setEnabled(false);
  _hardMonForm.optionsGroupBox->setEnabled(false);
  _hardMonForm.deviceStatusGroupBox->setEnabled(false);
  _hardMonForm.devicePropertiesGroupBox->setEnabled(false);

  // The following widgets are diabled because they are not implemented yet
  _hardMonForm.continuousReadCheckBox->setEnabled(false);
  _hardMonForm.writeToFileButton->setEnabled(false);
  _hardMonForm.readFromFileButton->setEnabled(false);

  // customize table display
  _customDelegate.setDoubleSpinBoxPrecision(_floatPrecision);
  _hardMonForm.valuesTableWidget->setItemDelegate(&_customDelegate);

  _plotWindow = new PlotWindow(this);

  // sorted enabled by default; calling this here will trigger the slot, which
  // sorts the register list.
  _hardMonForm.SortAscendingcheckBox->setChecked(true);


  connect(_hardMonForm.showPlotWindowCheckBox, SIGNAL(stateChanged(int)),
	  this, SLOT(showPlotWindow(int)));

  connect(_plotWindow, SIGNAL(plotWindowClosed()),
	  this, SLOT(unckeckShowPlotWindow()));

  // also the plot window dfunctions are only enabled when a device is opened.
   _plotWindow->setEnabled(false);
}

QtHardMon::~QtHardMon()
{}

void  QtHardMon::loadBoards()
{
  // Show a file dialog to select the dmap file.
  QString dmapFileName = QFileDialog::getOpenFileName(this,
						      tr("Open DeviceMap file"), 
						      ".", 
						      tr("DeviceMap files (*.dmap);; All files (*)"));

  if (dmapFileName.isEmpty())
  {
    // No file name selected, just quit.
    return;
  }

  // set the directory of the dmap file as the current working directory so 
  // relative map file pathes work
  QDir::setCurrent( QFileInfo(dmapFileName).absolutePath() );

  // The return value is intentionally ignored. Cast to void to suppress compiler warnings.
  // (I use c-type because it's shorter syntax and does not matter here).
  (void) loadDmapFile(dmapFileName);
}

bool  QtHardMon::loadDmapFile( QString const & dmapFileName )
{

  mtca4u::DMapFilesParser filesParser;

  try{
    filesParser.parse_file(dmapFileName.toStdString());
    BackendFactory::getInstance().setDMapFilePath(dmapFileName.toStdString());

  }
  catch( Exception & e )
  {
    QMessageBox messageBox;
    messageBox.setText("Could not load DeviceMap file "+dmapFileName+".");
    messageBox.setInformativeText(QString("Info: An exception was thrown:")+e.what());
    messageBox.exec();
    
    // We just return after displaying the message and leave the deviceList as it was.
    return false;
  }

  // store the dmap file name for further usage. The variable with the underscore is the class wide variable.
  _dmapFileName = dmapFileName;

  // clear the device list and the device specific info 
  _hardMonForm.deviceListWidget->clear();

  // Set the keyboard focus away from the deviceListWidget. This would trigger the deviceSelected 
  // on the first entry, which we don't want. The focus is set to the registerTreeWidget.
  _hardMonForm.registerTreeWidget->setFocus(Qt::OtherFocusReason);

  for (DMapFilesParser::iterator deviceIter = filesParser.begin();
       deviceIter != filesParser.end(); ++deviceIter)
  {
    
    // fixme: Issue in libdevMap: why is the iterator on a pair?
    _hardMonForm.deviceListWidget->addItem( new DeviceListItem( (*deviceIter).first, (*deviceIter).second, 
							       (*deviceIter).first.deviceName.c_str(),
							       _hardMonForm.deviceListWidget) );
  }

  return true;
  // on user request: do not automatically load the first device. This might be not accessible and
  // immediately gives an error message.
  //_hardMonForm.deviceListWidget->setCurrentRow(0);
}

void QtHardMon::deviceSelected(QListWidgetItem *deviceItem,
                               QListWidgetItem * /*previousDeviceItem */) {
  _hardMonForm.devicePropertiesGroupBox->setEnabled(true);

  // When the deviceListWidget is cleared , the currentItemChanged signal is
  // emitted with a null pointer. We have to catch this here and return. Before
  // returning we clear the device specific display info, close the device and
  // empty the register list.
  if (!deviceItem) {
    _hardMonForm.deviceNameDisplay->setText("");
    _hardMonForm.deviceFileDisplay->setText("");
    _hardMonForm.mapFileDisplay->setText("");
    _hardMonForm.mapFileDisplay->setToolTip("");
    _hardMonForm.deviceStatusGroupBox->setEnabled(false);
    _hardMonForm.devicePropertiesGroupBox->setEnabled(false);
    closeDevice();
    _hardMonForm.registerTreeWidget->clear();

    return;
  }

  _hardMonForm.deviceStatusGroupBox->setEnabled(true);
  _hardMonForm.devicePropertiesGroupBox->setEnabled(true);

  DeviceListItem *deviceListItem = static_cast<DeviceListItem *>(deviceItem);

  _currentDeviceListItem = deviceListItem;
  // close the previous device. This also disables the relevant GUI elements
  closeDevice();
  // The user has selected a new device. The old device pointer should be
  // cleared. Open device is intelligent to create a new object through the
  // factory before calling _mtcaDevice->open()
  _mtcaDevice.reset();
  // opening the device enables the gui elements if success
  openDevice(deviceListItem->getDeviceMapElement().deviceName);

  _hardMonForm.deviceNameDisplay->setText( deviceListItem->getDeviceMapElement().deviceName.c_str());
  _hardMonForm.deviceFileDisplay->setText( deviceListItem->getDeviceMapElement().uri.c_str());

  std::string absPath = deviceListItem->getDeviceMapElement().mapFileName;
  std::string mapFileName = extractFileNameFromPath(absPath);
  _hardMonForm.mapFileDisplay->setText(mapFileName.c_str());
  _hardMonForm.mapFileDisplay->setToolTip(absPath.c_str());

  populateRegisterTree(deviceItem);
}

void QtHardMon::openDevice( std::string const & deviceFileName ) //Change name to createAndOpenDevice();
{
  //try to open a createa and new device. If this fails disable the buttons and the registerValues
	try
  {
    // this might throw
  if(_mtcaDevice == NULL){
    // We work with the same instance on _mtcaDevice as long as the current
    // entry in the GUI device list is selected. _mtcaDevice is reset only when
    // the user moves to a new entry in the device list (see deviceSelected)
    _mtcaDevice = BackendFactory::getInstance().createBackend(deviceFileName);
  }
  _mtcaDevice->open();
    // enable all of the GUI in case it was deactivated before
    _hardMonForm.valuesTableWidget->setEnabled(true);
    _hardMonForm.operationsGroupBox->setEnabled(true);
    _hardMonForm.optionsGroupBox->setEnabled(true);
    _plotWindow->setEnabled(true);

    _hardMonForm.openClosedLabel->setText(
	 QApplication::translate("QtHardMonForm",
				 "Device is open.", 0,
				 QApplication::UnicodeUTF8));
    _hardMonForm.openCloseButton->setText(
	 QApplication::translate("QtHardMonForm", "Close", 0,
				 QApplication::UnicodeUTF8));
  }
  catch(Exception & e)
  {
    QMessageBox messageBox(QMessageBox::Warning, tr("QtHardMon: Warning"),
			   QString("Could not create the device ")+
			   deviceFileName.c_str()+".",
			   QMessageBox::Ok, this);
    messageBox.setInformativeText(QString("Info: An exception was thrown:")+e.what());
    messageBox.exec();
  }
}

void QtHardMon::closeDevice()
{
	if (_mtcaDevice)
		_mtcaDevice->close();
   _hardMonForm.valuesTableWidget->setEnabled(false);
   _hardMonForm.operationsGroupBox->setEnabled(false);
   _hardMonForm.optionsGroupBox->setEnabled(false);
   _plotWindow->setEnabled(false);
   // If the device is closed then there is no way we can read values from the
   // registers - they are not available anymore. Nothing to show on the table
   clearAllRowsInTable();
   _hardMonForm.openClosedLabel->setText(
		QApplication::translate("QtHardMonForm", "Device is closed.", 0,
					QApplication::UnicodeUTF8));
   _hardMonForm.openCloseButton->setText(
		QApplication::translate("QtHardMonForm", "Open", 0,
					QApplication::UnicodeUTF8));
}

void QtHardMon::registerSelected(QTreeWidgetItem * registerItem, QTreeWidgetItem * /*previousRegisterItem */)
{
  // There is a case when a device entry is clicked in the device list, the slot
  // is called with a NULL registerItem
  if (!registerItem) {
    clearGroupBoxDisplay();
    return;
  }

  CustomQTreeItem *registerTreeItem = static_cast<CustomQTreeItem *>(registerItem);
  RegisterPropertyGrpBox grpBoxInfo = getRegisterPropertyGrpBoxData();
  registerTreeItem->updateRegisterProperties(grpBoxInfo);

  // remember that this register has been selected
  if((registerTreeItem->type() != ModuleItem::DataType) && (registerTreeItem->type() != MultiplexedAreaItem::DataType)){
    _currentDeviceListItem->setLastSelectedRegisterName( registerTreeItem->getRegisterMapElement().name ) ;
    _currentDeviceListItem->setLastSelectedModuleName( registerTreeItem->getRegisterMapElement().module ) ;
  }

  if (!_autoRead || (registerTreeItem->type() == ModuleItem::DataType)){
    // If automatic reading is deactivated the widget has to be cleared so all widget items are empty.
    // In addition the write button is deactivated so the invalid items cannot be written to the register.
    _hardMonForm.valuesTableWidget->clearContents();
    _hardMonForm.valuesTableWidget->setRowCount(0);
    _hardMonForm.writeButton->setEnabled(false);
  } else {
    read();
  }
}

void QtHardMon::read()
{
  ++_insideReadOrWrite;

  CustomQTreeItem *registerTreeItem = static_cast<CustomQTreeItem *>(
      _hardMonForm.registerTreeWidget->currentItem());

  try {
    if (_mtcaDevice->isOpen()) {
      TableWidgetData tableData(_hardMonForm.valuesTableWidget, _maxWords,
                                _mtcaDevice);
      registerTreeItem->read(tableData);
      _hardMonForm.writeButton->setEnabled(true);
    }
  }
  catch (InvalidOperationException &e) {
    QMessageBox::warning(this, "QtHardMon read error", e.what());
    return;
  }
  catch (std::exception &e) {
    closeDevice();
    _hardMonForm.writeButton->setEnabled(false);
    // the error message accesses the _currentDeviceListItem. Is
    // this safe? It might be NULL.
    QMessageBox messageBox(
        QMessageBox::Critical, tr("QtHardMon: Error"),
        QString("Error reading from device ") +
            _currentDeviceListItem->getDeviceMapElement().uri.c_str() +
            ".",
        QMessageBox::Ok, this);
    messageBox.setInformativeText(QString("Info: An exception was thrown:") +
                                  e.what() +
                                  QString("\n\nThe device has been closed."));
    messageBox.exec();
  }

  // check if plotting after reading is requested
  if (_plotWindow->isVisible() && _plotWindow->plotAfterReadIsChecked()) {
    _plotWindow->plot();
  }

  --_insideReadOrWrite;
}

void QtHardMon::write()
{
  ++_insideReadOrWrite;

  CustomQTreeItem *registerTreeItem = static_cast<CustomQTreeItem *>(
      _hardMonForm.registerTreeWidget->currentItem());

  try {
    if (_mtcaDevice->isOpen()) {
      TableWidgetData tableData(_hardMonForm.valuesTableWidget, _maxWords,
                                _mtcaDevice);
      registerTreeItem->write(tableData);
    }
  }
  catch (InvalidOperationException &e) {
    QMessageBox::warning(this, "QtHardMon write error", e.what());
    return;
  }
  catch (std::exception &e) {
  		closeDevice();

    // the error message accesses the _currentDeviceListItem. Is this safe? It
    // might be NULL.
    QMessageBox messageBox(
        QMessageBox::Critical, tr("QtHardMon: Error"),
        QString("Error writing to device ") +
            _currentDeviceListItem->getDeviceMapElement().uri.c_str() +
            ".",
        QMessageBox::Ok, this);
    messageBox.setInformativeText(QString("Info: An exception was thrown:") +
                                  e.what() +
                                  QString("\n\nThe device has been closed."));
    messageBox.exec();
  }

  if (  _hardMonForm.readAfterWriteCheckBox->isChecked() )
  {
    read();
  }
  else{
    clearBackground();
  }

  --_insideReadOrWrite;
}

void QtHardMon::preferences()
{
  // create a preferences dialog and set the correct warning message with contains the default number for maxWords.
  QDialog preferencesDialog;
  Ui::PreferencesDialogForm preferencesDialogForm;
  preferencesDialogForm.setupUi(&preferencesDialog);
  preferencesDialogForm.maxWordsWarningLabel->setText(QString("WARNING:")+
						      " Setting this value too high can exhaust your memory, "+
						      "which will lead to a segmentation fault. Default value is "+
						      QString::number(DEFAULT_MAX_WORDS));

  preferencesDialogForm.fontSizeSpinBox->setValue(font().pointSize());
  preferencesDialogForm.autoReadCheckBox->setChecked(_autoRead);
  preferencesDialogForm.readOnClickCheckBox->setChecked(_readOnClick);

  // set up the current value of maxWords
  preferencesDialogForm.maxWordsSpinBox->setMaximum( INT_MAX );
  preferencesDialogForm.maxWordsSpinBox->setValue( _maxWords );

  // set up the floating point display decimal places
  preferencesDialogForm.precisionSpinBox->setMinimum(1); // minimum one decimal place display
  preferencesDialogForm.precisionSpinBox->setMaximum(10);// maximum 10 decimal places
  preferencesDialogForm.precisionSpinBox->setValue(_floatPrecision);

  int dialogResult = preferencesDialog.exec();

  // only set the values if ok has been pressed
  if (dialogResult == QDialog::Accepted )
  {
    _maxWords =  preferencesDialogForm.maxWordsSpinBox->value();

    // Read and set precision for delegate class
    _floatPrecision = preferencesDialogForm.precisionSpinBox->value();
    _customDelegate.setDoubleSpinBoxPrecision(_floatPrecision);

    // call registerSelected() so the size of the valuesList is adapted and possible missing values are read
    // from the device
    registerSelected(_hardMonForm.registerTreeWidget->currentItem(),_hardMonForm.registerTreeWidget->currentItem());

    QFont newFont(this->font());
    newFont.setPointSize(preferencesDialogForm.fontSizeSpinBox->value());
    QApplication::setFont(newFont);

    _readOnClick = preferencesDialogForm.readOnClickCheckBox->isChecked();
    _autoRead = preferencesDialogForm.autoReadCheckBox->isChecked();
  }
  
}

void QtHardMon::aboutQtHardMon()
{
  QMessageBox::about(this, "About QtHardMon", QString("QtHardMon Version ")+QT_HARD_MON_VERSION);
}

void QtHardMon::aboutQt()
{
  QMessageBox::aboutQt(this, "About Qt");
}

void QtHardMon::loadConfig()
{
  // the local, temporary variable, without underscore
  QString configFileName = QFileDialog::getOpenFileName(this,
							tr("Open DeviceMap file"), 
							".",
							tr("HardMon config files (*.cfg);; All files (*)"));

  if (configFileName.isEmpty())
  {
    // No file name selected, just quit.
    return;
  }

  loadConfig(configFileName);
}

void QtHardMon::loadConfig(QString const & configFileName)
{

  ConfigFileReaderWriter configReader;
  try{
    // even the constructor can throw when opening and reading a file
    configReader.read(configFileName.toStdString());
  }
  catch( std::ifstream::failure & e)
   {
     QMessageBox messageBox(QMessageBox::Warning, tr("QtHardMon: Warning"),
			    QString("Could not read config file ")+ 
			    configFileName+".",
			    QMessageBox::Ok, this);
     messageBox.setInformativeText(QString("Info: An exception was thrown:")+e.what());
     messageBox.exec();

     return;
   }

  // At this point we considder the config file as successfully loaded, even though it might not contain any
  // valid parameters. But we store the file name.
  _configFileName = configFileName;

  // show message box with parse errors, but just continue normally
  if (!configReader.getBadLines().isEmpty() )
  {
     QMessageBox messageBox(QMessageBox::Warning, tr("QtHardMon: Warning"),
			    QString("The following lines from the config file ")+
			    configFileName+" are invalid and will be ignored. Please fix your config file.\n",
			    QMessageBox::Ok, this);
     QString infoText;
     for (QStringList::const_iterator badLinesIter = configReader.getBadLines().begin();
	  badLinesIter != configReader.getBadLines().end(); ++badLinesIter)
     {
       infoText += *badLinesIter;
       infoText += "\n";
     }
     
     messageBox.setInformativeText(infoText);
     messageBox.exec();    
  }


  // first handle all settings that do not depend on opening a device map

  _floatPrecision = configReader.getValue(PRECISION_INDICATOR_STRING, static_cast<int>(_floatPrecision)); // is check necessary? should display default
  _customDelegate.setDoubleSpinBoxPrecision(_floatPrecision);

  // store in a local variable for now
  int maxWords = configReader.getValue(MAX_WORDS_STRING, static_cast<int>(_maxWords));
  // check for validity. Minimum reasonable value is 1.
  if (maxWords >=1)
  {
     //only after checking set the class wide maxWords variable
    _maxWords = static_cast<unsigned int>(maxWords);
     // Update the register, so the length of the valuesList is adapted.
     //If another register is loaded from the config this might be repeated.
     //But for an easier logic we take this little overhead.
     registerSelected(_hardMonForm.registerTreeWidget->currentItem(),
		      _hardMonForm.registerTreeWidget->currentItem());
  }
  else
  {
    QMessageBox::warning(this,  tr("QtHardMon: Warning"), QString("Read invalid maxWords from config file."));
  }
  
  int readAfterWriteFlag = configReader.getValue(READ_AFTER_WRITE_STRING, 
						 _hardMonForm.readAfterWriteCheckBox->isChecked() ? 1 : 0);
  // we use the implicit conversion 0=false, everyting else is true
  _hardMonForm.readAfterWriteCheckBox->setChecked( readAfterWriteFlag );

  int showPlotWindowFlag =  configReader.getValue(SHOW_PLOT_WINDOW_STRING,
						 _hardMonForm.showPlotWindowCheckBox->isChecked() ? 1 : 0);
  _hardMonForm.showPlotWindowCheckBox->setChecked( showPlotWindowFlag );

  int plotAfterReadFlag =  configReader.getValue(PLOT_AFTER_READ_STRING,
						 _plotWindow->plotAfterReadIsChecked() ? 1 : 0);
   _plotWindow->setPlotAfterRead( plotAfterReadFlag );

   int autoselectPreviousRegisterFlag = configReader.getValue(
       AUTOSELECT_PREVIOUS_REGISTER_STRING,
       _hardMonForm.autoselectPreviousRegisterCheckBox->isChecked() ? 1 : 0);
   _hardMonForm.autoselectPreviousRegisterCheckBox->setChecked(
       autoselectPreviousRegisterFlag);

   _readOnClick = static_cast<bool>( configReader.getValue(READ_ON_CLICK_STRING, _readOnClick ? 1 : 0 ) );
   _autoRead = static_cast<bool>( configReader.getValue(AUTO_READ_STRING, _autoRead ? 1 : 0 ) );

   int fontSize = configReader.getValue( FONT_SIZE_STRING, font().pointSize() );
   // Check validity of the font size.
   if (fontSize < 1 || fontSize > 99)
   {
   }
   else
   {
     QFont newFont(this->font());
     newFont.setPointSize(fontSize);
     QApplication::setFont(newFont);
   }
    
  // now read the mapping file, device and register. If anything goes wrong we can just exit the function because
  // all other variables have already been processed.
  std::string dmapFileString = configReader.getValue(DMAP_FILE_STRING, std::string());

  if (dmapFileString.empty())
  {
    // no dmap file. Just return, also loading device and register does not make sense without dmap
    return;
  }

  if (!loadDmapFile(dmapFileString.c_str()) )
  {
    // Just return, also loading device and register does not make sense without dmap
    return;
  }

  //loop all devices and try to determine the last used module and register
  for (int deviceRow = 0; deviceRow < _hardMonForm.deviceListWidget->count(); ++deviceRow)
  {
    DeviceListItem * deviceListItem =  
      static_cast<DeviceListItem *>(_hardMonForm.deviceListWidget->item(deviceRow) );
    
    // determine the module and the register 
    std::string deviceRegisterString = deviceListItem->getDeviceMapElement().deviceName + REGISTER_EXTENSION_STRING;
    std::string registerName = configReader.getValue(deviceRegisterString, std::string() );

    std::string deviceModuleString = deviceListItem->getDeviceMapElement().deviceName + MODULE_EXTENSION_STRING;
    std::string moduleName = configReader.getValue(deviceModuleString, std::string() );
    
    deviceListItem->setLastSelectedRegisterName(registerName);
    deviceListItem->setLastSelectedModuleName(moduleName);
  }// for deviceRow
  

  // search for the device string 
  std::string currentDeviceString = configReader.getValue(CURRENT_DEVICE_STRING, std::string());

  if (currentDeviceString.empty())
  {
    // no device specified, noting left to do
    return;
  }
  
  // try to find the device in the list
  QList<QListWidgetItem *> matchingDevices = _hardMonForm.deviceListWidget->findItems(currentDeviceString.c_str(),
										      Qt::MatchExactly);

  if (matchingDevices.isEmpty())
  {
    // the item should have been there, give a warning
    QMessageBox::warning(this, tr("QtHardMon: Warning"), QString("Device ") + currentDeviceString.c_str()
			 + " is not in the dmap file.");
    return;
  }

  // It's safe to call static_cast because the deviceListWidget is controlled by this application.
  // We know that it is a deviceListItem, no nynamic checking needed.
  // We also know that matchingDevices.begin() is valid because the list is not empty.
  // In case there is more than one entry with the same name we just pick the first one.
  DeviceListItem *deviceListItem = static_cast< DeviceListItem *>(*matchingDevices.begin());

  // now we are ready to select the device
  _hardMonForm.deviceListWidget->setCurrentItem(deviceListItem);

}

void QtHardMon::saveConfig()
{
  if (_configFileName.isEmpty())
  {
    saveConfigAs();
  }
  else
  {
    writeConfig(_configFileName);
  }
}

void QtHardMon::saveConfigAs()
{
  // the local, temporary variable, without underscore
  QString configFileName = QFileDialog::getSaveFileName(this,
							tr("Open DeviceMap file"), 
							".",
							tr("HardMon config files (*.cfg);; All files (*)"));

  if (configFileName.isEmpty())
  {
    // No file name selected, just quit.
    return;
  }
  
  // The file name seems valid. Now try to write it and store the name in the class wide variable if successful.
  try{
    writeConfig(configFileName);
  }
  catch( std::ifstream::failure & e)
  {
    // a message box with the error warning has already been displayed. Nothing more to do but quit.
    return;
  }

  // only store the file name upon successful writing (to the class wide variable with the unterscore)
  _configFileName = configFileName;
}

void QtHardMon::writeConfig(QString const & fileName)
{
  ConfigFileReaderWriter configWriter;
  
  // set the variables we want do write to file

  // only write the file name when it's not empty, otherwise we get an invalid line
  if( ! _dmapFileName.isEmpty() )
  {
    configWriter.setValue(DMAP_FILE_STRING, _dmapFileName.toStdString());
  }

  // The device list widget only contains deviceListItems, so it's safe to use a static cast here.
  DeviceListItem * deviceListItem =  static_cast<DeviceListItem *>(_hardMonForm.deviceListWidget->currentItem() );

     // the device list might be emtpy, or there is no current item (?, is this possibe?) 
     
  if (deviceListItem)
  {
    configWriter.setValue( CURRENT_DEVICE_STRING, deviceListItem->getDeviceMapElement().deviceName );
    // writing a register without item does not make sense, so we keep it in the if block
    //    configWriter.setValue(CURRENT_REGISTER_ROW_STRING, _hardMonForm.registerTreeWidget->currentRow());
  }
  
  // add a value to store the last register for each device
  for (int deviceRow = 0; deviceRow < _hardMonForm.deviceListWidget->count(); ++deviceRow)
  {
    deviceListItem = static_cast<DeviceListItem *>(_hardMonForm.deviceListWidget->item(deviceRow) );
    
    // Only write to the config file if the 'last selected' strings are not empty.
    // Empty strings would cause a parse error, and if the entry is not found it falls back to empty string anyway.
    if( !deviceListItem->getLastSelectedRegisterName().empty() ){
      std::string deviceRegisterString 
	= deviceListItem->getDeviceMapElement().deviceName + REGISTER_EXTENSION_STRING;
      configWriter.setValue(deviceRegisterString, deviceListItem->getLastSelectedRegisterName());
    }

    if( !deviceListItem->getLastSelectedModuleName().empty() ){
      std::string deviceModuleString = deviceListItem->getDeviceMapElement().deviceName + MODULE_EXTENSION_STRING;
      configWriter.setValue(deviceModuleString, deviceListItem->getLastSelectedModuleName());
    }
  }

  configWriter.setValue(MAX_WORDS_STRING, static_cast<int>(_maxWords));
  configWriter.setValue(PRECISION_INDICATOR_STRING, static_cast<int>(_floatPrecision));
  configWriter.setValue(READ_AFTER_WRITE_STRING,  _hardMonForm.readAfterWriteCheckBox->isChecked() ? 1 : 0 );
  configWriter.setValue(SHOW_PLOT_WINDOW_STRING,  _hardMonForm.showPlotWindowCheckBox->isChecked() ? 1 : 0 );
  configWriter.setValue(AUTOSELECT_PREVIOUS_REGISTER_STRING,
                        _hardMonForm.autoselectPreviousRegisterCheckBox->isChecked() ? 1 : 0);
  configWriter.setValue(PLOT_AFTER_READ_STRING, _plotWindow->plotAfterReadIsChecked() ? 1 : 0 );
  configWriter.setValue(FONT_SIZE_STRING, font().pointSize());
  configWriter.setValue(AUTO_READ_STRING, _autoRead ? 1 : 0 );
  configWriter.setValue(READ_ON_CLICK_STRING, _readOnClick ? 1 : 0 );

  // this 
   try{
     configWriter.write(fileName.toStdString());
   }
   // we catch a write failure here to show a message box, but rethrow the exception
   catch( std::ifstream::failure & e)
   {
     QMessageBox messageBox(QMessageBox::Critical, tr("QtHardMon: Error"),
			    QString("Could not write config file ")+ 
			    fileName+".",
			    QMessageBox::Ok, this);
     messageBox.setInformativeText(QString("Info: An exception was thrown:")+e.what());
     messageBox.exec();

     // rethrow the exception so the calling code knows that writing failed. 
     throw;
   }   
}

void QtHardMon::showPlotWindow(int checkState)
{
  if (checkState ==Qt::Unchecked)
  {
    _plotWindow->setVisible(false);
  }
  else
  {
    _plotWindow->plot();
    _plotWindow->setVisible(true);
  }
}

void QtHardMon::unckeckShowPlotWindow()
{
  // the plot window just closed. Uncheck the showPlotWindow check box
  _hardMonForm.showPlotWindowCheckBox->setChecked(false);
}

// The constructor itself is empty. It just calls the construtor of the mother class and the copy
// constructors of the data members
QtHardMon::DeviceListItem::DeviceListItem( mtca4u::DeviceInfoMap::DeviceInfo const & device_map_emlement,
					   mtca4u::RegisterInfoMapPointer const & register_map_pointer,
					   QListWidget * parent_ )
  : QListWidgetItem(parent_, DeviceListItemType), _deviceMapElement( device_map_emlement ),
    _registerInfoMapPointer( register_map_pointer ),_lastSelectedRegisterName(),_lastSelectedModuleName()
  
{}

QtHardMon::DeviceListItem::DeviceListItem( mtca4u::DeviceInfoMap::DeviceInfo const & device_map_emlement,
					   mtca4u::RegisterInfoMapPointer const & register_map_pointer,
					   const QString & text_, QListWidget * parent_ )
  : QListWidgetItem(text_, parent_, DeviceListItemType), _deviceMapElement( device_map_emlement ),
    _registerInfoMapPointer( register_map_pointer ),_lastSelectedRegisterName(),_lastSelectedModuleName()
{}

QtHardMon::DeviceListItem::DeviceListItem( mtca4u::DeviceInfoMap::DeviceInfo const & device_map_emlement,
					   mtca4u::RegisterInfoMapPointer const & register_map_pointer,
					   const QIcon & icon_, const QString & text_, QListWidget * parent_ )
  : QListWidgetItem(icon_, text_, parent_, DeviceListItemType),
    _deviceMapElement( device_map_emlement ),  _registerInfoMapPointer( register_map_pointer ),
    _lastSelectedRegisterName(),_lastSelectedModuleName()
{}

QtHardMon::DeviceListItem::~DeviceListItem(){}

 mtca4u::DeviceInfoMap::DeviceInfo const & QtHardMon::DeviceListItem::getDeviceMapElement() const
{
  return _deviceMapElement;
}

mtca4u::RegisterInfoMapPointer const & QtHardMon::DeviceListItem::getRegisterMapPointer() const
{
  return _registerInfoMapPointer;
}

std::string QtHardMon::DeviceListItem::getLastSelectedRegisterName() const
{
  return _lastSelectedRegisterName;
}

void QtHardMon::DeviceListItem::setLastSelectedRegisterName(std::string const & registerName)
{
  _lastSelectedRegisterName = registerName;
}

std::string QtHardMon::DeviceListItem::getLastSelectedModuleName() const
{
  return _lastSelectedModuleName;
}

void QtHardMon::DeviceListItem::setLastSelectedModuleName(std::string const & moduleName)
{
  _lastSelectedModuleName = moduleName;
}

void QtHardMon::registerClicked(QTreeWidgetItem * /*registerItem*/) {
  // Do not execute the read if the corresponding flag is off
  // registerSelected method.
  if (!_readOnClick) {
    //    std::cout << "Ignoring click" <<std::endl;
    return;
  }

  read();
}

void QtHardMon::openCloseDevice(){
	if (!_mtcaDevice)
		openDevice( _currentDeviceListItem->getDeviceMapElement().deviceName );
	else
	{
	 if (_mtcaDevice->isOpen()){
    closeDevice();
  }else{
    openDevice( _currentDeviceListItem->getDeviceMapElement().deviceName );
  }
	}
}

void QtHardMon::updateTableEntries(int row, int column) {

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

void QtHardMon::changeBackgroundIfModified( int row, int column ){
  if (_insideReadOrWrite==0){
    _hardMonForm.valuesTableWidget->item(row, column)->setBackground( _modifiedBackgroundBrush );
  }
  else{
    clearRowBackgroundColour(row);
  }
}

void QtHardMon::clearBackground(){
  int nRows = _hardMonForm.valuesTableWidget->rowCount();

  for( int row=0; row < nRows; ++row ){
    clearRowBackgroundColour(row);
  }
}

void QtHardMon::parseArgument(QString const &fileName) {
  if (checkExtension(fileName, ".dmap") == true) {
    (void)loadDmapFile(fileName);
  } else if (checkExtension(fileName, ".cfg") == true) {
    loadConfig(fileName);
  } else {
    QMessageBox messageBox(
        QMessageBox::Warning, tr("QtHardMon: Warning"),
        QString(
            "Unsupported file extension provided. Filename will be ignored."),
        QMessageBox::Ok, this);
    messageBox.exec();
  }
}

bool QtHardMon::checkExtension(QString const &fileName, QString extension) {
  QStringRef extensionOfProvidedFile(
      &fileName, (fileName.size() - extension.size()), extension.size());
  bool areStringsEqual = (extension.compare(extensionOfProvidedFile) == 0);
  return areStringsEqual;
}

double QtHardMon::convertToDouble(int decimalValue) {
  FixedPointConverter converter = getConverter();
  return converter.toDouble(decimalValue);
}

int QtHardMon::convertToFixedPoint(double doubleValue) {
  FixedPointConverter converter = getConverter();
  return converter.toFixedPoint(doubleValue);
}

void QtHardMon::clearRowBackgroundColour(int row) {
  int numberOfColumns = getNumberOfColumsInTableWidget();
  for (int columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
    if (isValidCell(row, columnIndex)) {
      clearCellBackground(row, columnIndex);
    }
  }
}

template <typename T> void QtHardMon::writeCell(int row, int column, T value) {
  QTableWidgetItem *widgetItem = new QTableWidgetItem();
  QVariant dataVariant;
  dataVariant.setValue(value);
  widgetItem->setData(Qt::DisplayRole, dataVariant);
  _hardMonForm.valuesTableWidget->setItem(row, column, widgetItem);
}

template <typename T> T QtHardMon::readCell(int row, int column) {
  return (_hardMonForm.valuesTableWidget->item(row, column)
              ->data(Qt::DisplayRole)
              .value<T>());
}

mtca4u::FixedPointConverter QtHardMon::getConverter() {
	CustomQTreeItem *registerInformation = dynamic_cast<CustomQTreeItem *>(
      _hardMonForm.registerTreeWidget->currentItem());
  if (!registerInformation){
    QMessageBox::warning(this, "QtHardMon internal error", "Could not create fixed point converter for current register.");
    return FixedPointConverter();
  }
  
  return registerInformation->getFixedPointConverter();
}

int QtHardMon::getNumberOfColumsInTableWidget() {
  return (_hardMonForm.valuesTableWidget->columnCount());
}

bool QtHardMon::isValidCell(int row, int columnIndex) {
  return (_hardMonForm.valuesTableWidget->item(row, columnIndex) != NULL);
}

void QtHardMon::clearCellBackground(int row, int columnIndex) {
  _hardMonForm.valuesTableWidget->item(row, columnIndex)
      ->setBackground(_defaultBackgroundBrush);
}

bool QtHardMon::isMultiplexedDataRegion(
    const std::string &registerName) {
  if (registerName.substr(0, mtca4u::MULTIPLEXED_SEQUENCE_PREFIX.size()) ==
      mtca4u::MULTIPLEXED_SEQUENCE_PREFIX) {
    return true;
  }
    return false;
}

bool QtHardMon::isSeqDescriptor(const std::string &registerName) {
  if (registerName.substr(0, mtca4u::SEQUENCE_PREFIX.size()) ==
      mtca4u::SEQUENCE_PREFIX) {
    return true;
  }
  return false;
}

CustomQTreeItem *QtHardMon::createAreaDesciptor(
    const DeviceListItem *deviceListItem,
    mtca4u::RegisterInfoMap::RegisterInfo const &regInfo) {

  std::string registerName = regInfo.name;
  std::string regionName = extractMultiplexedRegionName(registerName);
  std::string moduleName = regInfo.module;
  mtca4u::RegisterInfoMapPointer RegisterInfoMap = deviceListItem->getRegisterMapPointer();

  boost::shared_ptr<MultiplexedDataAccessor<double> > accessor =
      mtca4u::MultiplexedDataAccessor<double>::createInstance(
          regionName, moduleName, _mtcaDevice, RegisterInfoMap);

  return (new MultiplexedAreaItem(accessor, regInfo, registerName.c_str()) );
}

CustomQTreeItem *QtHardMon::createAreaDescriptorSubtree(
    CustomQTreeItem *areaDescriptor, mtca4u::RegisterInfoMap::const_iterator &it,
    mtca4u::RegisterInfoMap::const_iterator finalIterator) {
  // FIXME: this is not nice
  ++it; // start from the first seq description
  for (unsigned int sequenceCount = 0;
       ((it != finalIterator) && isSeqDescriptor(it->name));
       ++it, ++sequenceCount) {
    CustomQTreeItem *seq = new SequenceDescriptor(
        *it, sequenceCount, it->name.c_str());
    areaDescriptor->addChild(seq);
  }
  --it; // leave the iterator at the last sequence description
  return areaDescriptor;
}

std::string QtHardMon::extractMultiplexedRegionName(
    const std::string &regName) {
  if (isMultiplexedDataRegion(regName)) {
    return (regName.substr(mtca4u::MULTIPLEXED_SEQUENCE_PREFIX.size()));
  } else {
    return "";
  }
}

RegisterPropertyGrpBox QtHardMon::getRegisterPropertyGrpBoxData() {

  RegisterPropertyGrpBox grpBoxData;
  grpBoxData.registerNameDisplay = _hardMonForm.registerNameDisplay;
  grpBoxData.moduleDisplay = _hardMonForm.moduleDisplay;
  grpBoxData.registerBarDisplay = _hardMonForm.registerBarDisplay;
  grpBoxData.registerAddressDisplay = _hardMonForm.registerAddressDisplay;
  grpBoxData.registerNElementsDisplay = _hardMonForm.registerNElementsDisplay;
  grpBoxData.registerSizeDisplay = _hardMonForm.registerSizeDisplay;
  grpBoxData.registerWidthDisplay = _hardMonForm.registerWidthDisplay;
  grpBoxData.registerFracBitsDisplay = _hardMonForm.registerFracBitsDisplay;
  grpBoxData.registeSignBitDisplay = _hardMonForm.registeSignBitDisplay;
  return grpBoxData;
}

void QtHardMon::clearGroupBoxDisplay() {
  _hardMonForm.registerNameDisplay->setText("");
  _hardMonForm.moduleDisplay->setText("");
  _hardMonForm.registerBarDisplay->setText("");
  _hardMonForm.registerNElementsDisplay->setText("");
  _hardMonForm.registerAddressDisplay->setText("");
  _hardMonForm.registerSizeDisplay->setText("");
  _hardMonForm.registerWidthDisplay->setText("");
  _hardMonForm.registerFracBitsDisplay->setText("");
  _hardMonForm.registeSignBitDisplay->setText("");
}

void QtHardMon::clearAllRowsInTable() {
  _hardMonForm.valuesTableWidget->clearContents();
  int nRows = 0;
  _hardMonForm.valuesTableWidget->setRowCount(nRows);
}

std::string QtHardMon::extractFileNameFromPath(const std::string &fileName) {
  std::string extractedName = fileName;
  size_t position = fileName.find_last_of('/');
  if (position != std::string::npos) {
    extractedName = fileName.substr(position + 1, std::string::npos);
  }
  return extractedName;
}

void QtHardMon::populateRegisterTree(QListWidgetItem *deviceItem) {

  if (deviceItem == NULL) {
    return;
  }

  // the deviceItem actually is a DeviceListItemType. As this is a private slot
  // it is safe to assume this
  // and use a static cast.
  DeviceListItem *deviceListItem = static_cast<DeviceListItem *>(deviceItem);
  _hardMonForm.registerTreeWidget->clear();

  // get the registerMap and fill the RegisterTreeWidget
  for (RegisterInfoMap::const_iterator registerIter =
           deviceListItem->getRegisterMapPointer()->begin();
       registerIter != deviceListItem->getRegisterMapPointer()->end();
       ++registerIter) {
    QString moduleName(registerIter->module.empty()
                           ? NO_MODULE_NAME_STRING
                           : registerIter->module.c_str());
    QList<QTreeWidgetItem *> moduleList =
        _hardMonForm.registerTreeWidget->findItems(moduleName,
                                                   Qt::MatchExactly);

    QTreeWidgetItem *moduleItem;
    if (moduleList.empty()) {
      moduleItem = new ModuleItem(QString(moduleName));
      _hardMonForm.registerTreeWidget->addTopLevelItem(moduleItem);
    } else {
      moduleItem = moduleList.front(); // should be safe
    }

    if (isMultiplexedDataRegion(registerIter->name)) {
      CustomQTreeItem *areaDescriptor =
          createAreaDesciptor(deviceListItem, *registerIter);
      RegisterInfoMap::const_iterator it_end =
          deviceListItem->getRegisterMapPointer()->end();
      areaDescriptor =
          createAreaDescriptorSubtree(areaDescriptor, registerIter, it_end);
      moduleItem->addChild(dynamic_cast<QTreeWidgetItem*>(areaDescriptor));
    } else {
      moduleItem->addChild(
          new RegisterItem(*registerIter, registerIter->name.c_str()));
    }
  }
  _hardMonForm.registerTreeWidget->expandAll();

  // In case the read on select option is enabled, selecting the previously
  // active register on the device triggers an implicit read as well.
  // The user may now opt to not select the last active
  // register (and hence avoid the implicit read on this register when
  // the card is selected)
  if (_hardMonForm.autoselectPreviousRegisterCheckBox->isChecked()) {

    // Searching a sub-tree does not work in QTreeWidget. So here is the
    // strategy:
    // Get a list of all registers with this name.
    QList<QTreeWidgetItem *> registerList =
        _hardMonForm.registerTreeWidget->findItems(
            deviceListItem->getLastSelectedRegisterName().c_str(),
            Qt::MatchExactly | Qt::MatchRecursive);

    // Iterate the list until we find the one with the right module
    for (QList<QTreeWidgetItem *>::iterator registerIter = registerList.begin();
         registerIter != registerList.end(); ++registerIter) {
      CustomQTreeItem *registerItem =
          static_cast<CustomQTreeItem *>(*registerIter);
      // if we found the right register select it and quit the loop
      if (registerItem->getRegisterMapElement().module ==
          deviceListItem->getLastSelectedModuleName()) {
        _hardMonForm.registerTreeWidget->setCurrentItem(registerItem);
        break;
      }
    } // for registerIter
  }   // if autoselectPreviousRegister
}

void QtHardMon::addCopyActionForTableWidget() {
  QAction *copy = new QAction(tr("&Copy"), _hardMonForm.valuesTableWidget);
  copy->setShortcuts(QKeySequence::Copy);
  copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(copy, SIGNAL(triggered()), this, SLOT(copyTableDataToClipBoard()));
  _hardMonForm.valuesTableWidget->addAction(copy);
}

void QtHardMon::addCopyActionForRegisterTreeWidget() {
  QAction *copy = new QAction(tr("&Copy"), _hardMonForm.registerTreeWidget);
  copy->setShortcuts(QKeySequence::Copy);
  copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(copy, SIGNAL(triggered()), this,
          SLOT(copyRegisterTreeItemNameToClipBoard()));
  _hardMonForm.registerTreeWidget->addAction(copy);
}

void QtHardMon::copyRegisterTreeItemNameToClipBoard() {
  QTreeWidgetItem *currentItem = _hardMonForm.registerTreeWidget->currentItem();
  if (currentItem) {
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->clear(QClipboard::Clipboard);
    // clear the 'selection clipboard'; basically content that you get on
    // center mouse click
    clipboard->clear(QClipboard::Selection);

    QString name= currentItem->text(0);
    clipboard->setText(name);
  }
  return;
}

void QtHardMon::copyTableDataToClipBoard(){
	//TODO: SOmething for later. Not Implemented yet
}

void QtHardMon::handleSortCheckboxClick(int state) {
  if (state == Qt::Checked) {
    _hardMonForm.registerTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    _hardMonForm.registerTreeWidget->setSortingEnabled(true);
  } else if (state == Qt::Unchecked) {
    _hardMonForm.registerTreeWidget->setSortingEnabled(false);
    // redraw the currently sorted tree to pick up the order from the mapfile.
    populateRegisterTree(_hardMonForm.deviceListWidget->currentItem());
  }
}
