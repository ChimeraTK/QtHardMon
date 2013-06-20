#ifndef QT_HARD_MON
#define QT_HARD_MON

#include "ui_QtHardMonForm.h"
#include <QIcon>

class QtHardMon: public QMainWindow
{
 Q_OBJECT

 public:
  QtHardMon(QWidget * parent = 0, Qt::WindowFlags flags = 0);
  ~QtHardMon();

  // slots for internal use in this class. We do not provide a widget to be used inside another widget.
 private slots:
  /*  void readFromFile();
  void saveToFile();
  void exportToFile();
  void read();//< Read register from device.
  void write();//< Read register to device.
  void plot();//< Plot the register content in a separate window.
  */
  void loadBoards();
  void deviceSelected(QListWidgetItem * deviceItem);

 private:
  
  Ui::QtHardMonForm hardMonForm;
};

#endif// QT_HARD_MON
