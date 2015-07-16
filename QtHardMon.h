#ifndef QT_HARD_MON
#define QT_HARD_MON

#include "ui_QtHardMonForm.h"
#include "PlotWindow.h"
#include "CustomDelegates.h"
#include <QIcon>
#include <QDir>
#include <QStyledItemDelegate>

#include <MtcaMappedDevice/dmapFilesParser.h>
#include <MtcaMappedDevice/devPCIE.h>
#include <MtcaMappedDevice/FixedPointConverter.h>

#include "CustomQTreeItem.h"
#include <boost/shared_ptr.hpp>

/** The QtHadMon class which implements all the GUI functionality.
 */
class QtHardMon: public QMainWindow
{
 Q_OBJECT

 public:
  /** The constructor */
  QtHardMon(QWidget * parent_ = 0, Qt::WindowFlags flags = 0);
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
  void registerSelected(QTreeWidgetItem * registerItem, QTreeWidgetItem * /*previousRegisterItem */ = NULL);
  void registerClicked(QTreeWidgetItem * registerItem); ///< Executed if a register is clicked

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
  void updateTableEntries( int row, int column );

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

  // Disable copy constructor and assignment operator
  // This is the main class and it should'nt need copying
  // TODO: confirm this assumption
  Q_DISABLE_COPY(QtHardMon) // Easy way to get around -Weffc++ warning:
                            // class QtHardMon’ has pointer data members -
                            // but does not overide copy constructor and
                            // assignment operator


  
  Ui::QtHardMonForm _hardMonForm; ///< The GUI form which hold all the widgets.
  boost::shared_ptr <mtca4u::devBase> _mtcaDevice; ///< The instance of the device which is being accessed.
  unsigned int _maxWords; ///< The maximum number of words displayed in the values list.
  unsigned int _floatPrecision; ///< Decimal places to be shown for values in the double column
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
  CustomDelegates _customDelegate;///< provides display customizations for the table widget.

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

  /** A helper class to store listWidgetItems which also contain the dmapElem and ptrmapFile information.
   */
  class DeviceListItem: public QListWidgetItem
  {
    public:
      /** The simplest cvonstructor, no text or icon for the entry*/
      DeviceListItem ( mtca4u::dmapFile::dmapElem const & device_map_emlement, mtca4u::ptrmapFile const & register_map_pointer,
		       QListWidget * parent_ = 0 );

      /** Constructor which sets the text entry in the list. */
      DeviceListItem ( mtca4u::dmapFile::dmapElem const & device_map_emlement, mtca4u::ptrmapFile const & register_map_pointer,
		       const QString & text_, QListWidget * parent_ = 0 );

      /** Constructor which sets the text entry in the list and an icon. */      
      DeviceListItem ( mtca4u::dmapFile::dmapElem const & device_map_emlement, mtca4u::ptrmapFile const & register_map_pointer,
		       const QIcon & icon_, const QString & text_, QListWidget * parent_ = 0 );

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

      /** Get the name of the last register which had been selected.
       */
      std::string getLastSelectedRegisterName() const;

      /** Set the last register which had been selected.
       */
      void setLastSelectedRegisterName(std::string const & registerName);

      /** Get the name of the last register's module which had been selected.
       */
      std::string getLastSelectedModuleName() const;

      /** Set the module of the last register which had been selected.
       */
      void setLastSelectedModuleName(std::string const & moduleName);

    private:
      mtca4u::dmapFile::dmapElem _deviceMapElement; ///< The instance of the DeviceMapElement
      mtca4u::ptrmapFile _registerMapPointer; ///< The instance of the RegisterMapPointer
      std::string _lastSelectedRegisterName; ///< The last selected register before the item was deselected
      std::string _lastSelectedModuleName; ///< The last selected register's module before the item was deselected
  };

  /** A helper class to store treeWidgetItems which also contain the mapElem information.
   */
  class RegisterTreeItem: public QTreeWidgetItem
  {
    public:
      /** Constructor for top level items */
      RegisterTreeItem ( mtca4u::mapFile::mapElem const & register_map_emlement,
			 const QString & text_, QTreeWidget * parent_ = 0 );

      /** Constructor for child items */
      RegisterTreeItem ( mtca4u::mapFile::mapElem const & register_map_emlement,
			 const QString & text_, QTreeWidgetItem * parent_ = 0);
      
      /** The destructor. Currently does nothing because the members go out of scope automatically. */
      virtual ~RegisterTreeItem();
      
      /** Returns a reference to the registerMapElement, i.e. the register information. */
      mtca4u::mapFile::mapElem const & getRegisterMapElement() const;
      
      /** The type of RegisterTreeItemType.  It's a user type because it is larger than QTreeWidgetItem::UserType.
       */
      static const int RegisterTreeItemType = QTreeWidgetItem::UserType + 1;

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
  double convertToDouble(int decimalValue);

  /*
   * Converts the input doubleValue to Fixed point int. Internally uses
   * mtca4u::FixedPointConverter
   */
  int convertToFixedPoint(double doubleValue);

  template<typename T>
  void writeCell(int row, int column, T value);
  template<typename T>
  T readCell (int row, int column);

  mtca4u::FixedPointConverter createConverter();
  int getNumberOfColumsInTableWidget();
  bool isValidCell(int row, int columnIndex);
  void clearCellBackground(int row, int columnIndex);
  void clearRowBackgroundColour(int row);
  bool isMultiplexedDataRegion(std::string const & registerName);
  bool isSeqDescriptor(std::string const & registerName);
  CustomQTreeItem *createAreaDesciptor(
      DeviceListItem const *deviceListItem,
      mtca4u::mapFile::mapElem const & regInfo);

  CustomQTreeItem *createAreaDescriptorSubtree(
      CustomQTreeItem *areaDescriptor,
      mtca4u::mapFile::const_iterator &currentIt,
      mtca4u::mapFile::const_iterator finalIterator);

  std::string extractMultiplexedRegionName(std::string const & regName);

  public:
};



#endif// QT_HARD_MON
