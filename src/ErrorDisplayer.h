#ifndef _QT_HARDMON_ERROR_DISPLAYER_H_
#define _QT_HARDMON_ERROR_DISPLAYER_H_

#include <string>
#include <QObject>

class ErrorDisplayer : public QObject{
  Q_OBJECT

public:
  ErrorDisplayer();
  
  void displayMessage(std::string const & errorMessage);
  bool suppressFurtherMessages;
  
protected:
  std::string _errorMessage;
  
protected slots:
  void doDisplay();

signals:
  void newMessageArrived();
};

#endif // _QT_HARDMON_ERROR_DISPLAYER_H_
