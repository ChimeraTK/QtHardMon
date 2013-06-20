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

  QString dmapFileName = QFileDialog::getOpenFileName(this,
						      tr("Open DeviceMap file"), 
						      "/home/jana", 
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
    hardMonForm.deviceListWidget->addItem((*deviceIter).first.dev_name.c_str());
  }

}

void QtHardMon::deviceSelected(QListWidgetItem * deviceItem)
{
  std::cout << "Device " << deviceItem->text().toStdString() << " selected." << std::endl;
  
}
