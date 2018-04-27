#include "ErrorDisplayer.h"
#include <QMessageBox>

ErrorDisplayer::ErrorDisplayer() : suppressFurtherMessages(false){
  connect(this, SIGNAL(newMessageArrived()), this, SLOT(doDisplay()));
}

void ErrorDisplayer::displayMessage(std::string const & errorMessage){
  // Always remember the last error message. The previous one might not have been
  // displayed yet because the signal was not processed yet. But as we only
  // show the window once, we want the latest message.
  _errorMessage=errorMessage;
 
  if (!suppressFurtherMessages){
    suppressFurtherMessages = true;
    emit newMessageArrived();
  }
}

void ErrorDisplayer::doDisplay(){
  if (!_errorMessage.empty()){
    QMessageBox::critical(nullptr, "QtHardMon Error", _errorMessage.c_str());
    _errorMessage="";
  }
}
