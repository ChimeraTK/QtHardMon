#include "QtHardMon.h"
#include "QtHardMonVersion.h"
#include <iostream>

#include <QMessageBox>
#include <QFileDialog>

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

QtHardMon::QtHardMon(QWidget * parent, Qt::WindowFlags flags) 
  : QMainWindow(parent, flags), _currentDeviceListItem(NULL)
{
  _hardMonForm.setupUi(this);

  setWindowTitle("QtHardMon");
  setWindowIcon(  QIcon(":/DESY_logo_nofade.png") );
  _hardMonForm.logoLabel->setPixmap( QPixmap(":/DESY_logo.png") );
  

  connect(_hardMonForm.deviceListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
	  this, SLOT( deviceSelected(QListWidgetItem *, QListWidgetItem *) ) );

  connect(_hardMonForm.registerListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
	  this, SLOT( registerSelected(QListWidgetItem *, QListWidgetItem *) ) );

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

  // The oparations and options group are disabled until a dmap file is loaded and a device has been opened 
  _hardMonForm.operationsGroupBox->setEnabled(false);
  _hardMonForm.optionsGroupBox->setEnabled(false);

  // The following widgets are diabled because they are not implemented yet
  _hardMonForm.hexValuesCheckBox->setEnabled(false);
  _hardMonForm.continuousReadCheckBox->setEnabled(false);
  _hardMonForm.readAfterWriteCheckBox->setEnabled(false);
  _hardMonForm.plotButton->setEnabled(false);
  _hardMonForm.writeToFileButton->setEnabled(false);
  _hardMonForm.readFromFileButton->setEnabled(false);
  _hardMonForm.readConfigAction->setEnabled(false);
  _hardMonForm.writeConfigAction->setEnabled(false);
}

QtHardMon::~QtHardMon()
{
}

void  QtHardMon::loadBoards()
{
  //  QMessageBox messageBox;

  dmapFilesParser filesParser;

  

  // fixme: remember last folder, ideally even after closing the HardMon
  QString dmapFileName = QFileDialog::getOpenFileName(this,
						      tr("Open DeviceMap file"), 
						      ".", 
						      tr("DeviceMap files (*.dmap) (*.dmap);; All files (*) (*)"));
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
    return;
  }

  _hardMonForm.deviceListWidget->clear();

  for (dmapFilesParser::iterator deviceIter = filesParser.begin();
       deviceIter != filesParser.end(); ++deviceIter)
  {
    
    // fixme: Issue in libdevMap: why is the iterator on a pair?
    _hardMonForm.deviceListWidget->addItem( new DeviceListItem( (*deviceIter).first, (*deviceIter).second, 
							       (*deviceIter).first.dev_name.c_str(),
							       _hardMonForm.deviceListWidget) );
  }

  // on user request: do not automatically load the first device. This might be not accessible and
  // immediately gives an error message.
  //_hardMonForm.deviceListWidget->setCurrentRow(0);
}

void QtHardMon::deviceSelected(QListWidgetItem * deviceItem, QListWidgetItem * /*previousDeviceItem */)
{
  // When the deviceListWidget is cleared , the currentItemChanged signal is emitted with a null pointer.
  // We have to catch this here and just do nothing.
  if (!deviceItem )
  {
      return;
  }

  //std::cout << "Device " << deviceItem->text().toStdString() << " selected." << std::endl;

  // the deviceItem actually is a DeviceListItemType. As this is a private slot it is save to assume this
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

  //close the previous device
  _mtcaDevice.closeDev();
  
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
    
    // We diable the buttons and registerValues and return.
    _hardMonForm.valuesTableWidget->setEnabled(false);
    _hardMonForm.operationsGroupBox->setEnabled(false);
    _hardMonForm.optionsGroupBox->setEnabled(false);

    // we cannot exit here because the last selected row still has to be restored
    //return;    
  }

  // enable all of the GUI in case it was deactivated before
  if ( _mtcaDevice.isOpen())
  {
    _hardMonForm.valuesTableWidget->setEnabled(true);
    _hardMonForm.operationsGroupBox->setEnabled(true);
    _hardMonForm.optionsGroupBox->setEnabled(true);
  }

  _hardMonForm.registerListWidget->setCurrentRow( deviceListItem->getLastSelectedRegisterRow() );
}

void QtHardMon::registerSelected(QListWidgetItem * registerItem, QListWidgetItem * /*previousRegisterItem */)
{
  // When the registerListWidget is cleared , the currentItemChanged signal is emitted with a null pointer.
  // We have to catch this here and just do nothing.
  if (!registerItem )
  {
      return;
  }

  //std::cout << "Register " << registerItem->text().toStdString() << " selected." << std::endl;

  // the registerItem actually is a RegisterListItemType. As this is a private slot it is save to assume this
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

  _hardMonForm.valuesTableWidget->setRowCount( registerListItem->getRegisterMapElement().reg_elem_nr );
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

  // In order to fill all following rows with -1 in case of a read error, but not try to do any
  // further read attempts, we introduce a status variable.
  bool readError=false;

  for (int row=0; row < registerListItem->getRegisterMapElement().reg_elem_nr; row++)
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
    dataItem->setData( 0, QVariant( registerContent ) ); // 0 is the default role
    _hardMonForm.valuesTableWidget->setItem(row, 0, dataItem );
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

  for (int row=0; row < registerListItem->getRegisterMapElement().reg_elem_nr; row++)
  {
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
 
}

void QtHardMon::aboutQtHardMon()
{
  QMessageBox::about(this, "About QtHardMon", QString("QtHardMon Version ")+QT_HARD_MON_VERSION);
}

void QtHardMon::aboutQt()
{
  QMessageBox::aboutQt(this, "About Qt");
}

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

