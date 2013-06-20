#include <QtGui>
#include "QtHardMon.h"

int main(int argv, char **args)
{
  QApplication app(argv, args);
  
  QtHardMon hardMon;
  hardMon.show();

  return app.exec();
}
