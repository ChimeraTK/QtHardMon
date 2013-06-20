#ifndef QT_HARD_MON
#define QT_HARD_MON

#include "ui_QtHardMonForm.h"
#include <QIcon>
#include <QDir>

#include <MtcaMappedDevice/dmapFilesParser.h>
#include <MtcaMappedDevice/devPCIE.h>

class QwtPlot;

/** The QtHadMon class which implements all the GUI functionality.
 */
class QtHardMon: public QMainWindow
{
 Q_OBJECT

 public:
  /** The constructor */
  QtHardMon(QWidget * parent = 0, Qt::WindowFlags flags = 0);
  /* The destructor. Need not be virtual because we have no virtual functions */
  ~QtHardMon();

 public:
  /** The function which actually performs the loading of the config file.
   */
  void loadConfig(QString const & configFileName);

  // slots for internal use in this class. We do not provide a widget to be used inside another widget.
 private slots:
  /*  void readFromFile();
  void saveToFile();
  */
  void plot();//< Plot the register content in a separate window.

  void read();//< Read register from device.
  void write();//< Read register to device.
  void loadBoards();

  void deviceSelected(QListWidgetItem * deviceItem, QListWidgetItem * /*previousDeviceItem */);
  void registerSelected(QListWidgetItem * deviceItem, QListWidgetItem * /*previousRegisterItem */);

  void aboutQtHardMon(); //< Show the aboutQtHardMon
  void aboutQt(); //< Show the aboutQt dialog

  void preferences(); //< Show the preferences dialog and set the according variables

  /** Load config from a file. This slot calls a file dialog and then accesses
   *  loadConfig(QString const & filename).
   */
  void loadConfig();

  /** Save the config to the same file. If no config was loaded or previously saved, saveConfigAs is called.
   */
  void saveConfig();

  /** Open a file dialog to query the config file name and save the config.
   */
  void saveConfigAs();

 private:
  
  Ui::QtHardMonForm _hardMonForm; //< The GUI form which hold all the widgets.
  devPCIE _mtcaDevice; //< The instance of the device which is being accessed.
  unsigned int _maxWords; //< The maximum number of words displayed in the values list.
  QString _dmapFileName; //< The file name of the last opened dmap file
  QString _configFileName; //< Name of the config file (last saved or read)

  /** Write the config to the given file name.
   */
  void writeConfig(QString const & fileName);

  /** Load a dmap file and return whether loading was successful. The return value can safely be ignored
   *  if the information is not required in the programme flow.
   */
  bool loadDmapFile(QString const & dmapFileName);

  /** Close the device and disable the access buttons.
   */
  void closeDevice();

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

      /* No assignment operator, the default is fine. */
      //DeviceListItem & operator=( const DeviceListItem & other );
      
      /** The destructor. Currently does nothing because the members go out of scope automatically. */
      virtual ~DeviceListItem();
      
      /** Returns a reference to the deviceMapElement, i.e. the device information. */
      dmapFile::dmapElem const & getDeviceMapElement() const;
      
      /** Returns a reference to the RegisterMapPointer (aka ptrmapFile) of this device. */
      ptrmapFile const & getRegisterMapPointer() const;
      
      /** The type of DeviceListItemType.  It's a user type because it is larger than QListWidgetItem::UserType.
       */
      static const int DeviceListItemType = QListWidgetItem::UserType + 1;

      /** Get the row of the last register which had been selected.
       */
      int getLastSelectedRegisterRow() const;

      /** Set the row of the last register which had been selected.
       */
      void setLastSelectedRegisterRow(int row);

    private:
      dmapFile::dmapElem _deviceMapElement; //< The instance of the DeviceMapElement
      ptrmapFile _registerMapPointer; //< The instance of the RegisterMapPointer
      int _lastSelectedRegisterRow; //< The last selected register before the item was deselected
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
      mapFile::mapElem const & getRegisterMapElement() const;
      
      /** The type of RegisterListItemType.  It's a user type because it is larger than QListWidgetItem::UserType.
       */
      static const int RegisterListItemType = QListWidgetItem::UserType + 2;

    private:
      mapFile::mapElem _registerMapElement; //< The instance of the RegisterMapElement
  };

  DeviceListItem * _currentDeviceListItem; //< Pointer to the currently selected deviceListItem

  // we keep this package dependent variable at the end to simpily the constructor initialiser list syntax
  // with the ifdef
#if(USE_QWT)
  QwtPlot *_qwtPlot; //< The instance of the qwt plot
  QDockWidget * _plotDock; //< The docable plot window
#endif

};

#endif// QT_HARD_MON
