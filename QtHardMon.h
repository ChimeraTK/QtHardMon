#ifndef QT_HARD_MON
#define QT_HARD_MON

#include "ui_QtHardMonForm.h"
#include <QIcon>

#include <MtcaMappedDevice/dmapFilesParser.h>

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

  /** A helper class to store listWidgetItems which also contain the dmapElem and ptrmapFile information.
   */
  class DeviceListItem: public QListWidgetItem
  {
    public:
      /** The simplest cvonstructor, no text or icon for the entry*/
      DeviceListItem ( dmapFile::dmapElem const & device_map_emlement, ptrmapFile const & register_map_pointer,
		       QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list. */
      DeviceListItem ( dmapFile::dmapElem const & device_map_emlement, ptrmapFile const & register_map_pointer,
		       const QString & text, QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list and an icon. */      
      DeviceListItem ( dmapFile::dmapElem const & device_map_emlement, ptrmapFile const & register_map_pointer,
		       const QIcon & icon, const QString & text, QListWidget * parent = 0 );

      /* No copy constructor, the default is fine. */
      //DeviceListItem ( const DeviceListItem & other );

      /** No assignment operator, the default is fine. */
      //DeviceListItem & operator=( const DeviceListItem & other );
      
      /** The destructor. Currently does nothing because the members go out of scope automatically. */
      virtual ~DeviceListItem();
      
      /** Returns a reference to the deviceMapElement, i.e. the device information. */
      dmapFile::dmapElem const & getDeviceMapElement();
      
      /** Returns a reference to the RegisterMapPointer (aka ptrmapFile) of this device. */
      ptrmapFile const & getRegisterMapPointer();
      
      /** The type of DeviceListItemType.  It's a user type because it is larger than QListWidgetItem::UserType.
       */
      static const int DeviceListItemType = QListWidgetItem::UserType + 1;

    private:
      dmapFile::dmapElem _deviceMapElement; //< The instance of the DeviceMapElement
      ptrmapFile _registerMapPointer; //< The instance of the RegisterMapPointer
      
  };

  /** A helper class to store listWidgetItems which also contain the mapElem information.
      FIXME: looks suspiciously like DeviceListItem -> code duplication. Make it template code?
   */
  class RegisterListItem: public QListWidgetItem
  {
    public:
      /** The simplest cvonstructor, no text or icon for the entry*/
      RegisterListItem ( mapFile::mapElem const & register_map_emlement,
		       QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list. */
      RegisterListItem ( mapFile::mapElem const & register_map_emlement,
		       const QString & text, QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list and an icon. */      
      RegisterListItem ( mapFile::mapElem const & register_map_emlement,
		       const QIcon & icon, const QString & text, QListWidget * parent = 0 );

      /* No copy constructor, the default is fine. */
      //RegisterListItem ( const RegisterListItem & other );

      /** No assignment operator, the default is fine. */
      //RegisterListItem & operator=( const RegisterListItem & other );
      
      /** The destructor. Currently does nothing because the members go out of scope automatically. */
      virtual ~RegisterListItem();
      
      /** Returns a reference to the registerMapElement, i.e. the register information. */
      mapFile::mapElem const & getRegisterMapElement();
      
      /** The type of RegisterListItemType.  It's a user type because it is larger than QListWidgetItem::UserType.
       */
      static const int RegisterListItemType = QListWidgetItem::UserType + 2;

    private:
      mapFile::mapElem _registerMapElement; //< The instance of the RegisterMapElement
      
  };

};

#endif// QT_HARD_MON
