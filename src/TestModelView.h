#ifndef TESTMODELVIEW_H
#define TESTMODELVIEW_H

#include "ui_TestModelViewForm.h"

class TestModelView: public QWidget{
 public:
  TestModelView(QWidget * parent=nullptr);
  
  Ui::TestModelViewForm ui;
};

#endif // TESTMODELVIEW_H
