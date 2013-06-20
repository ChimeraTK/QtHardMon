#include "QtHardMon.h"
#include <iostream>

#include <QMessageBox>
#include <QFileDialog>

#include <MtcaMappedDevice/exBase.h>
#include <MtcaMappedDevice/dmapFilesParser.h>

QtHardMon::QtHardMon(QWidget * parent, Qt::WindowFlags flags) 
  : QMainWindow(parent, flags)
{
  hardMonForm.setupUi(this);

  setWindowTitle("QtHardMon");
  setWindowIcon(  QIcon(":/MTCA4U_DESY_colors_lowRes.png") );
  hardMonForm.mtca4uLabel->setPixmap( QPixmap(":/MTCA4U_by_DESY_lowRes.png") );
  

  connect(hardMonForm.deviceListWidget, SIGNAL(itemActivated(QListWidgetItem *)), 
	  this, SLOT( deviceSelected(QListWidgetItem * ) ) );

  connect(hardMonForm.loadBoardsButton, SIGNAL(clicked()),
	  this, SLOT(loadBoards()));
					       

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
						      tr("DeviceMap files (*.dmap"));
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

  hardMonForm.deviceListWidget->clear();

  for (dmapFilesParser::iterator deviceIter = filesParser.begin();
       deviceIter != filesParser.end(); ++deviceIter)
  {
    
    // fixme: Issue in libdevMap: why is the iterator on a pair?
    hardMonForm.deviceListWidget->addItem( new DeviceListItem( (*deviceIter).first, (*deviceIter).second, 
							       (*deviceIter).first.dev_name.c_str(),
							       hardMonForm.deviceListWidget) );
  }

}

void QtHardMon::deviceSelected(QListWidgetItem * deviceItem)
{
  std::cout << "Device " << deviceItem->text().toStdString() << " selected." << std::endl;

  // the deviceItem actually is a DeviceListItemType. As this is a private slot it is save to assume this
  // and use a static cast.
  DeviceListItem * deviceListItem = static_cast<DeviceListItem *>(deviceItem);  
  
  // get the registerMap and fill the RegisterListWidget
  //  ptrmapFile const & rmp = 
  for (mapFile::const_iterator registerIter = deviceListItem->getRegisterMapPointer()->begin(); 
       registerIter != deviceListItem->getRegisterMapPointer()->end(); ++registerIter)
  {
    std::cout << *registerIter << std::endl;
    hardMonForm.registerListWidget->addItem( new RegisterListItem( *registerIter, registerIter->reg_name.c_str(),  
								   hardMonForm.registerListWidget ) );
  }

}

// The constructor itself is empty. It just calls the construtor of the mother class and the copy
// constructors of the data members
QtHardMon::DeviceListItem::DeviceListItem( dmapFile::dmapElem const & device_map_emlement, 
					   ptrmapFile const & register_map_pointer,
					   QListWidget * parent )
  : QListWidgetItem(parent, DeviceListItemType), _deviceMapElement( device_map_emlement ),
                                                 _registerMapPointer( register_map_pointer )
{}

QtHardMon::DeviceListItem::DeviceListItem( dmapFile::dmapElem const & device_map_emlement, 
					   ptrmapFile const & register_map_pointer,
					   const QString & text, QListWidget * parent )
  : QListWidgetItem(text, parent, DeviceListItemType), _deviceMapElement( device_map_emlement ),
	                                               _registerMapPointer( register_map_pointer )
{}

QtHardMon::DeviceListItem::DeviceListItem( dmapFile::dmapElem const & device_map_emlement, 
					   ptrmapFile const & register_map_pointer,
					   const QIcon & icon, const QString & text, QListWidget * parent )
  : QListWidgetItem(icon, text, parent, DeviceListItemType),
    _deviceMapElement( device_map_emlement ),  _registerMapPointer( register_map_pointer )
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

 dmapFile::dmapElem const & QtHardMon::DeviceListItem::getDeviceMapElement()
{
  return _deviceMapElement;
}

ptrmapFile const & QtHardMon::DeviceListItem::getRegisterMapPointer()
{
  return _registerMapPointer;
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

 mapFile::mapElem const & QtHardMon::RegisterListItem::getRegisterMapElement()
{
  return _registerMapElement;
}

