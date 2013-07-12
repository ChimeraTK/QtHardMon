#include "QtHardMon.h"
#include "QtHardMonVersion.h"
#include "ui_PreferencesForm.h"
#include "ConfigFileReaderWriter.h"

#include <iostream>
#include <limits>

#include <QMessageBox>
#include <QFileDialog>
#include <QDockWidget>

#include <MtcaMappedDevice/exBase.h>
#include <MtcaMappedDevice/dmapFilesParser.h>
#include <MtcaMappedDevice/exDevPCIE.h>

// FIXME: how to solve the problem of the word size? Should come from pci express. 
// => need to improve the api
static const size_t WORD_SIZE_IN_BYTES = 4;

// The default maximum for the number of words in a register.
// This limits the number of rows in the valuesTableWidget to avoid a segmentation fault if too much
// memory is requested.
static const size_t DEFAULT_MAX_WORDS = 0x10000;

// Some variables to avoid duplication and possible inconsistencies in the code.
// These strings are used in the config file
#define DMAP_FILE_STRING "dmapFile"
#define CURRENT_DEVICE_STRING "currentDevice"
#define CURRENT_REGISTER_ROW_STRING "currentRegisterRow"
#define MAX_WORDS_STRING "maxWords"
#define READ_AFTER_WRITE_STRING "readAfterWrite"
#define HEX_VALUES_STRING "hexValues"
#define SHOW_PLOT_WINDOW_STRING "showPlotWindow"
#define PLOT_AFTER_READ_STRING "plotAfterRead"
#define REGISTER_EXTENSION_STRING "_REGISTER"

QtHardMon::QtHardMon(QWidget * parent, Qt::WindowFlags flags) 
  : QMainWindow(parent, flags), _maxWords( DEFAULT_MAX_WORDS ), _currentDeviceListItem(NULL)
{
  _hardMonForm.setupUi(this);

  setWindowTitle("QtHardMon");
  setWindowIcon(  QIcon(":/DESY_logo_nofade.png") );
  _hardMonForm.logoLabel->setPixmap( QPixmap(":/DESY_logo.png") );
  

  connect(_hardMonForm.deviceListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
	  this, SLOT( deviceSelected(QListWidgetItem *, QListWidgetItem *) ) );

  connect(_hardMonForm.registerListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
	  this, SLOT( registerSelected(QListWidgetItem *, QListWidgetItem *) ) );

  connect(_hardMonForm.registerListWidget, SIGNAL(itemActivated(QListWidgetItem *)), 
	  this, SLOT( registerSelected(QListWidgetItem *) ) );

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

  // The oparations and options group are disabled until a dmap file is loaded and a device has been opened 
  _hardMonForm.operationsGroupBox->setEnabled(false);
  _hardMonForm.optionsGroupBox->setEnabled(false);

  // The following widgets are diabled because they are not implemented yet
  _hardMonForm.hexValuesCheckBox->setEnabled(false);
  _hardMonForm.continuousReadCheckBox->setEnabled(false);
  _hardMonForm.writeToFileButton->setEnabled(false);
  _hardMonForm.readFromFileButton->setEnabled(false);

  _plotWindow = new PlotWindow(this);

  connect(_hardMonForm.showPlotWindowCheckBox, SIGNAL(stateChanged(int)),
	  this, SLOT(showPlotWindow(int)));

  connect(_plotWindow, SIGNAL(plotWindowClosed()),
	  this, SLOT(unckeckShowPlotWindow()));

  // also the plot window dfunctions are only enabled when a device is opened.
   _plotWindow->setEnabled(false);
}

QtHardMon::~QtHardMon()
{
  // As the plot window has NULL as parent, we have to delete it here.
  delete  _plotWindow;
}

void  QtHardMon::loadBoards()
{
  // Show a file dialog to select the dmap file.
  QString dmapFileName = QFileDialog::getOpenFileName(this,
						      tr("Open DeviceMap file"), 
						      ".", 
						      tr("DeviceMap files (*.dmap) (*.dmap);; All files (*) (*)"));

  if (dmapFileName.isEmpty())
  {
    // No file name selected, just quit.
    return;
  }

  // The return value is intentionally ignored. Cast to void to suppress compiler warnings.
  // (I use c-type because it's shorter syntax and does not matter here).
  (void) loadDmapFile(dmapFileName);
}

bool  QtHardMon::loadDmapFile( QString const & dmapFileName )
{

  dmapFilesParser filesParser;  

  try{
    filesParser.parse_file(dmapFileName.toStdString());
  }
  catch( exBase & e )
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
  // on the first entry, which we don't want. The focus is set to the registerListWidget.
  _hardMonForm.registerListWidget->setFocus(Qt::OtherFocusReason);

  for (dmapFilesParser::iterator deviceIter = filesParser.begin();
       deviceIter != filesParser.end(); ++deviceIter)
  {
    
    // fixme: Issue in libdevMap: why is the iterator on a pair?
    _hardMonForm.deviceListWidget->addItem( new DeviceListItem( (*deviceIter).first, (*deviceIter).second, 
							       (*deviceIter).first.dev_name.c_str(),
							       _hardMonForm.deviceListWidget) );
  }

  return true;
  // on user request: do not automatically load the first device. This might be not accessible and
  // immediately gives an error message.
  //_hardMonForm.deviceListWidget->setCurrentRow(0);
}

void QtHardMon::deviceSelected(QListWidgetItem * deviceItem, QListWidgetItem * /*previousDeviceItem */)
{
  // When the deviceListWidget is cleared , the currentItemChanged signal is emitted with a null pointer.
  // We have to catch this here and return. Before returning we clear the device specific display info,
  // close the device and empty the register list.
  if (!deviceItem )
  {
    _hardMonForm.deviceNameDisplay->setText( "" );
    _hardMonForm.deviceFileDisplay->setText( "" );
    _hardMonForm.mapFileDisplay->setText( "" );
    closeDevice();
    _hardMonForm.registerListWidget->clear();
    return;
  }

  //std::cout << "Device " << deviceItem->text().toStdString() << " selected." << std::endl;

  // the deviceItem actually is a DeviceListItemType. As this is a private slot it is safe to assume this
  // and use a static cast.
  DeviceListItem * deviceListItem = static_cast<DeviceListItem *>(deviceItem);  
  _currentDeviceListItem = deviceListItem;

  _hardMonForm.deviceNameDisplay->setText( deviceListItem->getDeviceMapElement().dev_name.c_str() );
  _hardMonForm.deviceFileDisplay->setText( deviceListItem->getDeviceMapElement().dev_file.c_str() );
  _hardMonForm.mapFileDisplay->setText( deviceListItem->getDeviceMapElement().map_file_name.c_str() );

  _hardMonForm.registerListWidget->clear();
   
  // get the registerMap and fill the RegisterListWidget
  //  ptrmapFile const & rmp = 
  for (mapFile::const_iterator registerIter = deviceListItem->getRegisterMapPointer()->begin(); 
       registerIter != deviceListItem->getRegisterMapPointer()->end(); ++registerIter)
  {
    //std::cout << *registerIter << std::endl;
    _hardMonForm.registerListWidget->addItem( new RegisterListItem( *registerIter, registerIter->reg_name.c_str(),  
								   _hardMonForm.registerListWidget ) );
  }

  //close the previous device. This also disables the relevant GUI elements
  closeDevice();
  
  //try to open a new device. If this fails disable the buttons and the registerValues
  try
  {
    _mtcaDevice.openDev(  deviceListItem->getDeviceMapElement().dev_file );
  }
  catch(exDevPCIE & e)
  {
    QMessageBox messageBox(QMessageBox::Warning, tr("QtHardMon: Warning"),
			   QString("Could not open the device ")+ 
			   deviceListItem->getDeviceMapElement().dev_file.c_str()+".",
			   QMessageBox::Ok, this);
    messageBox.setInformativeText(QString("Info: An exception was thrown:")+e.what());
    messageBox.exec();
    
    // we cannot exit here because the last selected row still has to be restored
    //return;    
  }

  // enable all of the GUI in case it was deactivated before
  if ( _mtcaDevice.isOpen())
  {
    _hardMonForm.valuesTableWidget->setEnabled(true);
    _hardMonForm.operationsGroupBox->setEnabled(true);
    _hardMonForm.optionsGroupBox->setEnabled(true);
    _plotWindow->setEnabled(true);
  }

  _hardMonForm.registerListWidget->setCurrentRow( deviceListItem->getLastSelectedRegisterRow() );
}

void QtHardMon::closeDevice()
{
   _mtcaDevice.closeDev();
   _hardMonForm.valuesTableWidget->setEnabled(false);
   _hardMonForm.operationsGroupBox->setEnabled(false);
   _hardMonForm.optionsGroupBox->setEnabled(false);
   _plotWindow->setEnabled(false);
}

void QtHardMon::registerSelected(QListWidgetItem * registerItem, QListWidgetItem * /*previousRegisterItem */)
{
  // When the registerListWidget is cleared , the currentItemChanged signal is emitted with a null pointer.
  // We have to catch this here and return.
  // Before returning we have to clear the register specific display info and the values list.
  if (!registerItem )
  {
    _hardMonForm.registerNameDisplay->setText("");
    _hardMonForm.registerBarDisplay->setText("");
    _hardMonForm.registerNElementsDisplay->setText("");
    _hardMonForm.registerAddressDisplay->setText("");
    _hardMonForm.registerSizeDisplay->setText("");
    _hardMonForm.valuesTableWidget->clear();
    
    return;
  }

  //std::cout << "Register " << registerItem->text().toStdString() << " selected." << std::endl;

  // the registerItem actually is a RegisterListItemType. As this is a private slot it is safe to assume this
  // and use a static cast.
  RegisterListItem * registerListItem = static_cast<RegisterListItem *>(registerItem);  
  
  _hardMonForm.registerNameDisplay->setText(    registerListItem->getRegisterMapElement().reg_name.c_str() );
  _hardMonForm.registerBarDisplay->setText( QString::number( registerListItem->getRegisterMapElement().reg_bar ));
  _hardMonForm.registerNElementsDisplay->setText(  
				  QString::number( registerListItem->getRegisterMapElement().reg_elem_nr ));
  _hardMonForm.registerAddressDisplay->setText(  
				  QString::number( registerListItem->getRegisterMapElement().reg_address ));
  _hardMonForm.registerSizeDisplay->setText(  
				  QString::number( registerListItem->getRegisterMapElement().reg_size ));

  // remember that this register has been selected
  _currentDeviceListItem->setLastSelectedRegisterRow(  _hardMonForm.registerListWidget->currentRow() );

  // if the register is too large not all words are displayed.
  // Set the size of the list to maxWords + 1, so the last line can show "truncated"
  int nRows = ( registerListItem->getRegisterMapElement().reg_elem_nr >  _maxWords ? 
		   _maxWords + 1 :  registerListItem->getRegisterMapElement().reg_elem_nr );

  _hardMonForm.valuesTableWidget->setRowCount( nRows );
  read();
}

void QtHardMon::read()
{
  RegisterListItem * registerListItem =
    static_cast<RegisterListItem *>(  _hardMonForm.registerListWidget->currentItem() );

  if (!registerListItem)
  {
    QMessageBox::warning(this, "QtHardMon read error", "No current register. Check your mapping file.");
    return;
  }

  std::cout << "this is QtHardMon::read()" 
   << " reading register " <<  registerListItem->getRegisterMapElement().reg_name<< std::endl;

  // In order to fill all following rows with -1 in case of a read error, but not try to do any
  // further read attempts, we introduce a status variable.
  bool readError=false;

  for (unsigned int row=0; row < registerListItem->getRegisterMapElement().reg_elem_nr; row++)
  {
    int registerContent = -1;

    // Try reading from the file, but only when it's open.
    // This avoids provoking exceptions in the "normal" data flow if a device could not be opened.
    // One might want to browse the registers for theis size on an unopened device, e.g.
    if ( _mtcaDevice.isOpen() && !readError)
    {
      // try to read from the device. If this fails this really is a problem.
      try
      {
	_mtcaDevice.readReg( registerListItem->getRegisterMapElement().reg_address + row*WORD_SIZE_IN_BYTES,
			     &registerContent,
			     registerListItem->getRegisterMapElement().reg_bar );
      }
      catch(exDevPCIE & e)
      {
	// the error message accesses the _currentDeviceListItem. Is this safe? It might be NULL.
	    QMessageBox messageBox(QMessageBox::Critical, tr("QtHardMon: Error"),
			   QString("Error reading from device ")+ 
			   _currentDeviceListItem->getDeviceMapElement().dev_file.c_str()+".",
			   QMessageBox::Ok, this);
	    messageBox.setInformativeText(QString("Info: An exception was thrown:")+e.what());
	    messageBox.exec();

	    // Turn on the read error flag. No further read attempts on this register.
	    readError=true;
      }
    }
    
    // Prepare a data item with a QVariant. The QVariant takes care that the type is recognised as
    // int and a proper editor (spin box) is used when editing in the GUI.
    QTableWidgetItem * dataItem =  new QTableWidgetItem();

    if (row == _maxWords)
    { // The register is too large to display. Show that it is truncated and stop reading
      dataItem->setText("truncated");
      // turn off the editable and selectable flags for the "truncated" entry
      dataItem->setFlags( dataItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable );
      dataItem->setToolTip("List is truncated. You can change the number of words displayed in the preferences.");
      _hardMonForm.valuesTableWidget->setItem(row, 0, dataItem );
      break;
    }

    dataItem->setData( 0, QVariant( registerContent ) ); // 0 is the default role
    _hardMonForm.valuesTableWidget->setItem(row, 0, dataItem );

  }// for row
 
  // check if plotting after reading is requested
  if (_plotWindow->isVisible() && _plotWindow->plotAfterReadIsChecked())
  {
    _plotWindow->plot();
  }
}

void QtHardMon::write()
{
  RegisterListItem * registerListItem =
    static_cast<RegisterListItem *>(  _hardMonForm.registerListWidget->currentItem() );

  if (!registerListItem)
  {
    QMessageBox::warning(this, "QtHardMon read error", "No current register. Check your mapping file.");
    return;
  }

  for (unsigned int row=0; row < registerListItem->getRegisterMapElement().reg_elem_nr; row++)
  {
    // if the register is too large only write the valid entries from the display list.
    if (row == _maxWords)
    {
      break;
    }

    int registerContent =  _hardMonForm.valuesTableWidget->item(row,0)->data(0 /*default role*/).toInt();

    // Try writing to the file only when it's open.
    // This should only be callable if the device could be opened, but zou never know.
    if ( _mtcaDevice.isOpen() )
    {
      // try to write to the device. If this fails this really is a problem.
      try
      {
	_mtcaDevice.writeReg( registerListItem->getRegisterMapElement().reg_address + row*WORD_SIZE_IN_BYTES,
			      registerContent,
			      registerListItem->getRegisterMapElement().reg_bar );
      }
      catch(exDevPCIE & e)
      {
	// the error message accesses the _currentDeviceListItem. Is this safe? It might be NULL.
	    QMessageBox messageBox(QMessageBox::Critical, tr("QtHardMon: Error"),
			   QString("Error writing to device ")+ 
			   _currentDeviceListItem->getDeviceMapElement().dev_file.c_str()+".",
			   QMessageBox::Ok, this);
	    messageBox.setInformativeText(QString("Info: An exception was thrown:")+e.what());
	    messageBox.exec();

      }//catch

    }//if isOpen

  }// for row

  if (  _hardMonForm.readAfterWriteCheckBox->isChecked() )
  {
    read();
  }

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

  // set up the current value of maxWords
  preferencesDialogForm.maxWordsSpinBox->setMaximum( INT_MAX );
  preferencesDialogForm.maxWordsSpinBox->setValue( _maxWords );

  int dialogResult = preferencesDialog.exec();

  // only set the values if ok has been pressed
  if (dialogResult == QDialog::Accepted )
  {
    _maxWords =  preferencesDialogForm.maxWordsSpinBox->value();
    // call registerSelected() so the size of the valuesList is adapted and possible missing values are read
    // from the device
    registerSelected(_hardMonForm.registerListWidget->currentItem(),_hardMonForm.registerListWidget->currentItem());

    QFont newFont(this->font());
    newFont.setPointSize(preferencesDialogForm.fontSizeSpinBox->value());
    QApplication::setFont(newFont);
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
							tr("HardMon config files (*.cfg) (*.cfg);; All files (*) (*)"));

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
//    if (configReader.getBadLines().begin() != configReader.getBadLines().end() )
//      {
//	std::cout << "begin and end are different" << std::endl;
//      }
//    else
//      {
//	std::cout << "begin and end are the same" << std::endl;
//      }
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
     registerSelected(_hardMonForm.registerListWidget->currentItem(),
		      _hardMonForm.registerListWidget->currentItem());
  }
  else
  {
    QMessageBox::warning(this,  tr("QtHardMon: Warning"), QString("Read invalid maxWords from config file."));
  }
  
  int readAfterWriteFlag = configReader.getValue(READ_AFTER_WRITE_STRING, 
						 _hardMonForm.readAfterWriteCheckBox->isChecked() ? 1 : 0);
  // we use the implicit conversion 0=false, everyting else is true
  _hardMonForm.readAfterWriteCheckBox->setChecked( readAfterWriteFlag );

  int hexValuesFlag =  configReader.getValue(HEX_VALUES_STRING,
						 _hardMonForm.hexValuesCheckBox->isChecked() ? 1 : 0);
  _hardMonForm.hexValuesCheckBox->setChecked( hexValuesFlag );

  int showPlotWindowFlag =  configReader.getValue(SHOW_PLOT_WINDOW_STRING,
						 _hardMonForm.showPlotWindowCheckBox->isChecked() ? 1 : 0);
  _hardMonForm.showPlotWindowCheckBox->setChecked( showPlotWindowFlag );

  int plotAfterReadFlag =  configReader.getValue(PLOT_AFTER_READ_STRING,
						 _plotWindow->plotAfterReadIsChecked() ? 1 : 0);
   _plotWindow->setPlotAfterRead( plotAfterReadFlag );

    
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

  //loop all devices and try to determine the last used register
  for (int deviceRow = 0; deviceRow < _hardMonForm.deviceListWidget->count(); ++deviceRow)
  {
    DeviceListItem * deviceListItem =  
      static_cast<DeviceListItem *>(_hardMonForm.deviceListWidget->item(deviceRow) );
    
    std::string deviceRegisterString = deviceListItem->getDeviceMapElement().dev_name + REGISTER_EXTENSION_STRING;

    // we put 0 as default, which should be a safe value because that is set already
    int registerRow = configReader.getValue(deviceRegisterString, 0);

    // We just ignore negative values. The user can use this undocumented "feature" to comment out
    // the entry (although using a comment line is preferred).
    if (registerRow > 0)
    {
      // check that the requested row is not too large
      if (static_cast<unsigned int>(registerRow) >= deviceListItem->getRegisterMapPointer()->getMapFileSize())
      {
	QMessageBox messageBox(QMessageBox::Warning, tr("QtHardMon: Warning"),
			       QString(deviceRegisterString.c_str()) + " = "
			       + QString::number(registerRow)
			       + " is too large for device "+
			       deviceListItem->getDeviceMapElement().dev_name.c_str() +".",
			       QMessageBox::Ok, this);
	messageBox.setInformativeText( QString("Mapping file ") + 
				       deviceListItem->getDeviceMapElement().map_file_name.c_str() + 
				       " only has " +
				       QString::number( deviceListItem->getRegisterMapPointer()->getMapFileSize() )+
				       " entries." );
	messageBox.exec();
      }
      else
      {
	// currentRegisterRow is ok, set it in the device list item
	deviceListItem->setLastSelectedRegisterRow(registerRow);
      }
    }// if (registerRow > 0)
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
  DeviceListItem * deviceListItem = static_cast< DeviceListItem *>(*matchingDevices.begin());

  //START_OBSOLETE: reading the CURRENT_REGISTER_ROW_STRING is only kept for backward compatibility
  //Before selecting the device we try to read and set the correct register
  int currentRegisterRow =  configReader.getValue(CURRENT_REGISTER_ROW_STRING, -1);
  
  // We just ignore negative values. The user can use this undocumented "feature" to comment out
  // the entry (although using a comment line is preferred).
  if (currentRegisterRow > 0)
  {
    // check that the requested row is not too large
    if (static_cast<unsigned int>(currentRegisterRow) >= deviceListItem->getRegisterMapPointer()->getMapFileSize())
    {
      QMessageBox messageBox(QMessageBox::Warning, tr("QtHardMon: Warning"),
			     QString("currentRegisterRow=") + QString::number(currentRegisterRow)
			     + " is too large for device "+  currentDeviceString.c_str() +".",
			     QMessageBox::Ok, this);
      messageBox.setInformativeText( QString("Mapping file ") + 
				     deviceListItem->getDeviceMapElement().map_file_name.c_str() + 
				    " only has " +
				     QString::number( deviceListItem->getRegisterMapPointer()->getMapFileSize() )+
				     " entries." );
      messageBox.exec();
    }
    else
    {
      // currentRegisterRow is ok, set it in the device list item
      deviceListItem->setLastSelectedRegisterRow(currentRegisterRow);
    }
  }
  //END_OBSOLOETE

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
							tr("HardMon config files (*.cfg) (*.cfg);; All files (*) (*)"));

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
    configWriter.setValue( CURRENT_DEVICE_STRING, deviceListItem->getDeviceMapElement().dev_name );
    // writing a register without item does not make sense, so we keep it in the if block
    //    configWriter.setValue(CURRENT_REGISTER_ROW_STRING, _hardMonForm.registerListWidget->currentRow());
  }
  
  // add a value to store the last register for each device
  for (int deviceRow = 0; deviceRow < _hardMonForm.deviceListWidget->count(); ++deviceRow)
  {
    DeviceListItem * deviceListItem =  
      static_cast<DeviceListItem *>(_hardMonForm.deviceListWidget->item(deviceRow) );
    
    std::string deviceRegisterString = deviceListItem->getDeviceMapElement().dev_name + REGISTER_EXTENSION_STRING;
    configWriter.setValue(deviceRegisterString, deviceListItem->getLastSelectedRegisterRow());
  }

  configWriter.setValue(MAX_WORDS_STRING, static_cast<int>(_maxWords));
  configWriter.setValue(READ_AFTER_WRITE_STRING,  _hardMonForm.readAfterWriteCheckBox->isChecked() ? 1 : 0 );
  configWriter.setValue(HEX_VALUES_STRING,  _hardMonForm.hexValuesCheckBox->isChecked() ? 1 : 0 );
  configWriter.setValue(SHOW_PLOT_WINDOW_STRING,  _hardMonForm.showPlotWindowCheckBox->isChecked() ? 1 : 0 );
  configWriter.setValue(PLOT_AFTER_READ_STRING, _plotWindow->plotAfterReadIsChecked() ? 1 : 0 );

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
    _plotWindow->setVisible(true);    
  }
}

void QtHardMon::unckeckShowPlotWindow()
{
  // the plot window just closed. Uncheck the showPlotWindow check box
  _hardMonForm.showPlotWindowCheckBox->setChecked(false);
}

//void QtHardMon::plot()
//{
//#if(USE_QWT)
//  QVector<QPointF> samples;
//
//  //FIXME: use a data vector here. This also overcomes the truncation limitation (wanted?)
//  //Use the minimum of rowCount and maxWords. for truncated lists the last entry is invalid
//  for (int row = 0; row < std::min(_hardMonForm.valuesTableWidget->rowCount(), static_cast<int>(_maxWords)); ++row)
//  {
//    QTableWidgetItem *tableWidgetItem = _hardMonForm.valuesTableWidget->item(row,0); // always column 0
//    if (!tableWidgetItem)
//    {
//      // strange, this should not happen. print a warning message end stop plotting
//      QMessageBox::critical(this, tr("QtHardMon: Error creating plot"), QString("Value in row ")
//			    +QString::number(row) + " does not exist.");
//      return;
//    }
//
//    bool conversionOk;
//    // The 0 in data(0) is the policy.
//    int value = tableWidgetItem->data(0).toInt(&conversionOk);
//    
//    if (!conversionOk)
//    {
//      QMessageBox::critical(this, tr("QtHardMon: Error creating plot"),
//			    QString("Value in row ")+QString::number(row) + " is invalid.");
//      return;
//    }
//
//    samples.push_back(QPoint(row, value));
//  }
//  
//  QwtPointSeriesData* myData = new QwtPointSeriesData;
//  myData->setSamples(samples);
// 
//  QwtPlotCurve *curve1 = new QwtPlotCurve("Curve 1");
//
//  // at this point the curve takes ownership of the data object
//  curve1->setData(myData);
//
//  // replace the current plot.
//  delete _qwtPlot;
//  _qwtPlot = new QwtPlot(_plotDock); 
//  _plotDock->setWidget(_qwtPlot);
// 
//  // at this point the curve is attached to the plot, which will delete it when it goes out of scope
//  curve1->attach(_qwtPlot);
// 
//  _qwtPlot->replot();
//#endif // USE_QWT
//}

// The constructor itself is empty. It just calls the construtor of the mother class and the copy
// constructors of the data members
QtHardMon::DeviceListItem::DeviceListItem( dmapFile::dmapElem const & device_map_emlement, 
					   ptrmapFile const & register_map_pointer,
					   QListWidget * parent )
  : QListWidgetItem(parent, DeviceListItemType), _deviceMapElement( device_map_emlement ),
    _registerMapPointer( register_map_pointer ), _lastSelectedRegisterRow(0)
  
{}

QtHardMon::DeviceListItem::DeviceListItem( dmapFile::dmapElem const & device_map_emlement, 
					   ptrmapFile const & register_map_pointer,
					   const QString & text, QListWidget * parent )
  : QListWidgetItem(text, parent, DeviceListItemType), _deviceMapElement( device_map_emlement ),
    _registerMapPointer( register_map_pointer ), _lastSelectedRegisterRow(0)
{}

QtHardMon::DeviceListItem::DeviceListItem( dmapFile::dmapElem const & device_map_emlement, 
					   ptrmapFile const & register_map_pointer,
					   const QIcon & icon, const QString & text, QListWidget * parent )
  : QListWidgetItem(icon, text, parent, DeviceListItemType),
    _deviceMapElement( device_map_emlement ),  _registerMapPointer( register_map_pointer ),
    _lastSelectedRegisterRow(0)
{}

// non need to implement this. It is excactly what the default does.
//QtHardMon::DeviceListItem::DeviceListItem( const DeviceListItem & other )
//  : QListWidgetItem( other ) ,  _deviceMapElement( other.device_map_emlement ),
//    _registerMapPointer( other.register_map_pointer )
//{
//}


QtHardMon::DeviceListItem::~DeviceListItem(){}

// non need to implement this. It is excactly what the default does.
//QtHardMon::DeviceListItem::operator=( const DeviceListItem & other )
//{
//  QListWidgetItem::operator=(other);
//  _deviceMapElement=other._deviceMapElement;
//  _registerMapPointer=other._registerMapPointer;
//}

 dmapFile::dmapElem const & QtHardMon::DeviceListItem::getDeviceMapElement() const
{
  return _deviceMapElement;
}

ptrmapFile const & QtHardMon::DeviceListItem::getRegisterMapPointer() const
{
  return _registerMapPointer;
}

int QtHardMon::DeviceListItem::getLastSelectedRegisterRow() const
{
  return _lastSelectedRegisterRow;
}

void QtHardMon::DeviceListItem::setLastSelectedRegisterRow(int row)
{
  _lastSelectedRegisterRow = row;
}

// The constructor itself is empty. It just calls the construtor of the mother class and the copy
// constructors of the data members
QtHardMon::RegisterListItem::RegisterListItem( mapFile::mapElem const & register_map_emlement, 
					       QListWidget * parent )
  : QListWidgetItem(parent, RegisterListItemType), _registerMapElement( register_map_emlement )
{}

QtHardMon::RegisterListItem::RegisterListItem( mapFile::mapElem const & register_map_emlement, 
					       const QString & text, QListWidget * parent )
  : QListWidgetItem(text, parent, RegisterListItemType), _registerMapElement( register_map_emlement )
{}

QtHardMon::RegisterListItem::RegisterListItem( mapFile::mapElem const & register_map_emlement, 
					       const QIcon & icon, const QString & text, QListWidget * parent )
  : QListWidgetItem(icon, text, parent, RegisterListItemType),
    _registerMapElement( register_map_emlement )
{}

// non need to implement this. It is excactly what the default does.
//QtHardMon::RegisterListItem::RegisterListItem( const RegisterListItem & other )
//  : QListWidgetItem( other ) ,  _registerMapElement( other.register_map_emlement )
//{
//}


QtHardMon::RegisterListItem::~RegisterListItem(){}

// non need to implement this. It is excactly what the default does.
//QtHardMon::RegisterListItem::operator=( const RegisterListItem & other )
//{
//  QListWidgetItem::operator=(other);
//  _registerMapElement=other._registerMapElement;
//}

 mapFile::mapElem const & QtHardMon::RegisterListItem::getRegisterMapElement() const
{
  return _registerMapElement;
}
