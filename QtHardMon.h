#ifndef QT_HARD_MON
#define QT_HARD_MON

#include "ui_QtHardMonForm.h"
#include "PlotWindow.h"
#include <QIcon>
#include <QDir>

#include <MtcaMappedDevice/dmapFilesParser.h>
#include <MtcaMappedDevice/devPCIE.h>
//#include <>


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

  /** The function which actually performs the loading of the config file.
   */
  void loadConfig(QString const & configFileName);

  /**
   * This method checks if QtHardMon executable has been called with a
   * valid configuration or dmap file as an argument. It checks
   * the extension of the passed in argument to distinguish the two file
   * types. Depending on the file type passed, the method invokes either
   * loadConfig or loadBoards internally.
   * A pop up window with a warning is displayed if the fileName
   * argument provided does not end with extension ".dmap" or ".cfg"
   */
  void parseArgument(QString const &fileName);

  // slots for internal use in this class. We do not provide a widget to be used inside another widget.
 private slots:
  /*  void readFromFile();
  void saveToFile();
  */
  void showPlotWindow(int checkState);///< Slot to toggle the plot window visibility. Needs to be int because the sender slot is int.

  void unckeckShowPlotWindow();///< Slot with void argument to uncheck the check box.

  void read();///< Read register from device.
  void write();///< Read register to device.
  void loadBoards();///< Read a dmap file

  /** The device has changed. Read the meta data for this device and select the last active recister in this
   *  device.
   */
  void deviceSelected(QListWidgetItem * deviceItem, QListWidgetItem * /*previousDeviceItem */);
  /** The register has changed. Read the meta data for this register and perform an auto-read if this is
   *  activated.
   */
  void registerSelected(QListWidgetItem * registerItem, QListWidgetItem * /*previousRegisterItem */ = NULL);
  void registerClicked(QListWidgetItem * registerItem); ///< Executed if a register is clicked

  void aboutQtHardMon(); ///< Show the aboutQtHardMon
  void aboutQt(); ///< Show the aboutQt dialog

  void preferences(); ///< Show the preferences dialog and set the according variables

  /** Load config from a file. This slot calls a file dialog and then accesses
   *  loadConfig(QString const & filename).
   */
  void loadConfig();

  /** React on the open/close button. Calls openDevice or closeDevice, depending whether the
   *  device is closed/open.
   */
  void openCloseDevice();

  /** Save the config to the same file. If no config was loaded or previously saved, saveConfigAs is called.
   */
  void saveConfig();

  /** Open a file dialog to query the config file name and save the config.
   */
  void saveConfigAs();

  /** Updates the hex value if the dec value changes */
  void updateHexIfDecChanged( int row, int column );

  /** Sets the background color of the cell, depending on whether the update is made by read (normal color)
   *  or manually by the user (red).
   */
  void changeBackgroundIfModified( int row, int column );

  /** Set all background to non modified color.
   */
  void clearBackground();
  /*
   * returns true if the fileName ends with the provided
   * extension else false
   */
  bool checkExtension(QString const &fileName, QString extension);

 private:
  
  Ui::QtHardMonForm _hardMonForm; ///< The GUI form which hold all the widgets.
  mtca4u::devPCIE _mtcaDevice; ///< The instance of the device which is being accessed.
  unsigned int _maxWords; ///< The maximum number of words displayed in the values list.
  bool _autoRead; ///< Flag whether to automatically read on register change
  bool _readOnClick; ///< Flag wheter to read on click in the register list
  QString _dmapFileName; ///< The file name of the last opened dmap file
  QString _configFileName; ///< Name of the config file (last saved or read)
  int _insideReadOrWrite; ///< Counter flag to indicate if the read or write function is being executed
  ////< (used to set background color). It is implemented as a counter because read can be called from write,
  ////< and if read would reset a bool to false, it would be wrong in write. Just adding and subtracting is 
  ///< easier than catching all possible use cases.
  QBrush _defaultBackgroundBrush; ///< Normal brush color if the item is not modified
  QBrush _modifiedBackgroundBrush; ///< Brush color if the item has been modified

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

  /** Open the device and update the GUI accordingly.
   */
  void openDevice(std::string const & deviceFileName );

  /** Clear the valuesTableWidget and restore the dec/hex headers.
   */
  void clearValuesTableWidget();

  /** A helper class to store listWidgetItems which also contain the dmapElem and ptrmapFile information.
   */
  class DeviceListItem: public QListWidgetItem
  {
    public:
      /** The simplest cvonstructor, no text or icon for the entry*/
      DeviceListItem ( mtca4u::dmapFile::dmapElem const & device_map_emlement, mtca4u::ptrmapFile const & register_map_pointer,
		       QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list. */
      DeviceListItem ( mtca4u::dmapFile::dmapElem const & device_map_emlement, mtca4u::ptrmapFile const & register_map_pointer,
		       const QString & text, QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list and an icon. */      
      DeviceListItem ( mtca4u::dmapFile::dmapElem const & device_map_emlement, mtca4u::ptrmapFile const & register_map_pointer,
		       const QIcon & icon, const QString & text, QListWidget * parent = 0 );

      /* No copy constructor, the default is fine. */
      //DeviceListItem ( const DeviceListItem & other );

      /* No assignment operator, the default is fine. */
      //DeviceListItem & operator=( const DeviceListItem & other );
      
      /** The destructor. Currently does nothing because the members go out of scope automatically. */
      virtual ~DeviceListItem();
      
      /** Returns a reference to the deviceMapElement, i.e. the device information. */
      mtca4u::dmapFile::dmapElem const & getDeviceMapElement() const;
      
      /** Returns a reference to the RegisterMapPointer (aka ptrmapFile) of this device. */
      mtca4u::ptrmapFile const & getRegisterMapPointer() const;
      
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
      mtca4u::dmapFile::dmapElem _deviceMapElement; ///< The instance of the DeviceMapElement
      mtca4u::ptrmapFile _registerMapPointer; ///< The instance of the RegisterMapPointer
      int _lastSelectedRegisterRow; ///< The last selected register before the item was deselected
  };

  /** A helper class to store listWidgetItems which also contain the mapElem information.
      FIXME: looks suspiciously like DeviceListItem -> code duplication. Make it template code?
   */
  class RegisterListItem: public QListWidgetItem
  {
    public:
      /** The simplest cvonstructor, no text or icon for the entry*/
      RegisterListItem ( mtca4u::mapFile::mapElem const & register_map_emlement,
		       QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list. */
      RegisterListItem ( mtca4u::mapFile::mapElem const & register_map_emlement,
		       const QString & text, QListWidget * parent = 0 );

      /** Constructor which sets the text entry in the list and an icon. */      
      RegisterListItem ( mtca4u::mapFile::mapElem const & register_map_emlement,
		       const QIcon & icon, const QString & text, QListWidget * parent = 0 );

      /* No copy constructor, the default is fine. */
      //RegisterListItem ( const RegisterListItem & other );

      /* No assignment operator, the default is fine. */
      //RegisterListItem & operator=( const RegisterListItem & other );
      
      /** The destructor. Currently does nothing because the members go out of scope automatically. */
      virtual ~RegisterListItem();
      
      /** Returns a reference to the registerMapElement, i.e. the register information. */
      mtca4u::mapFile::mapElem const & getRegisterMapElement() const;
      
      /** The type of RegisterListItemType.  It's a user type because it is larger than QListWidgetItem::UserType.
       */
      static const int RegisterListItemType = QListWidgetItem::UserType + 2;

    private:
      mtca4u::mapFile::mapElem _registerMapElement; ///< The instance of the RegisterMapElement.
  };

  DeviceListItem * _currentDeviceListItem; ///< Pointer to the currently selected deviceListItem

  PlotWindow * _plotWindow; ///< The plot window

  // The plot window is allowed to access the internal varaibles (mutex and register content) for plotting.
  // We allow this for performance reasons: A thread-safe getter for the data members would require one further
  // copying of the data, a plot function in the plot window class, which is called with a reference of the data 
  // pointer, would require unnecessarily long locking of the mutex.
  friend class PlotWindow;

  /*
   * Converts the input decimalValue to double. Internally uses
   * mtca4u::FixedPointConverter
   */
  double getFractionalValue(int decimalValue, RegisterListItem *registerInformation);

  /*
   * Converts the input doubleValue to Fixed point int. Internally uses
   * mtca4u::FixedPointConverter
   */
  int getFixedPointValue(double doubleValue, RegisterListItem *registerInformation);
};

#endif// QT_HARD_MON
