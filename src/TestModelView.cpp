#include "TestModelView.h"
#include "RegisterAccessorModel.h"
#include <QtWidgets/QApplication>

#include <ChimeraTK/Device.h>
using namespace ChimeraTK;

TestModelView::TestModelView(QWidget * parent):
  QWidget(parent){
  ui.setupUi(this);
}

int main(int argc, char *argv[]){
  QApplication a(argc, argv);

  setDMapFilePath("/home/killenb/DeviceAccess/tests/dummies.dmap");
  Device d;
  d.open("PCIE1");
  
  TestModelView tmv;
  //RegisterAccessorModel<int32_t> accessorModel(0);
  RegisterAccessorModel accessorModel(0, d.getTwoDRegisterAccessor<int32_t>("ADC/WORD_CLK_MUX"));
  tmv.ui.tableView->setModel( &accessorModel );
  tmv.connect(tmv.ui.readButton, SIGNAL(clicked()), &accessorModel, SLOT(read()));
  tmv.show();
  return a.exec();
}
