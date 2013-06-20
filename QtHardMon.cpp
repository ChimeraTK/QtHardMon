#include "QtHardMon.h"
#include <iostream>

#include <QMessageBox>

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
  QMessageBox messageBox;
  messageBox.setText("Only loading non-existent toy devices.");
  messageBox.setInformativeText("A file dialog to select the cmap-file will be implemented later.");
  messageBox.exec();
  
  hardMonForm.deviceListWidget->addItem("FirstADCBoard");
  hardMonForm.deviceListWidget->addItem("SecondADCBoard");
}

void QtHardMon::deviceSelected(QListWidgetItem * deviceItem)
{
  std::cout << "Device " << deviceItem->text().toStdString() << " selected." << std::endl;
  
}
