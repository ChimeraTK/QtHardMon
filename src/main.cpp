#include "QtHardMon.h"
#include <QtGui>

int main(int argv, char **args) {
  //Have a while loop that restarts QtHardMon in case of exceptions.
  //It displays a message box with the exception message, then restarts.
  while(true){
    try{
      // To have everything as clean and possible on restart we
      // have all instances inside the try block, so the objects are cleaned up
      // before the catch block.
      QApplication app(argv, args);
      
      QtHardMon hardMon;
      
      // Check if there are command line arguments. A config file name can be
      // processed.
      if (argv > 1){ // 0 is the command itself, so there have to be more than 1
        hardMon.parseArgument(args[1]);
      }
      
      hardMon.show();
      return app.exec();

    }catch(std::exception &e){
      //We need a valid app to display the message box
      QApplication app(argv, args);
      QMessageBox::critical(nullptr, "QtHardMon Error", QString("Caught an unhandled exception: ")+e.what()+"\n\n QtHardMon has to be restarted!");
      //The messagebox is somewhat special. It is not executed via app.exec().
      //Calling the latter would infinitely block here because there is nothing
      //running.

      //jump back into the loop that restarts the QtHardMon application.
      continue;
    }
  }

}
