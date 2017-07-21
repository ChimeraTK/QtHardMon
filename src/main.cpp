#include <QtGui>
#include "QtHardMon.h"

int main(int argv, char **args)
{
  QApplication app(argv, args);

  QtHardMon hardMon;

  // Check if there are command line arguments. A config file name can be processed.
  if (argv > 1) // 0 is the command itself, so there have to be more than 1
  {
    hardMon.parseArgument( args[1] );
  }

  hardMon.show();

  return app.exec();
}
