#ifndef QT_HARD_MON
#define QT_HARD_MON

#include "ui_QtHardMonForm.h"
#include "PlotWindow.h"
#include "CustomDelegates.h"
#include <QIcon>
#include <QDir>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include <qclipboard.h>
#include "PropertiesWidgetProvider.h"

#ifndef Q_MOC_RUN
#include <mtca4u/Device.h>
#include <boost/shared_ptr.hpp>
#endif

/** The QtHadMon class which implements all the GUI functionality.
 */
class QtHardMon: public QMainWindow
{
 Q_OBJECT

 public:
  /** The constructor */
  QtHardMon(bool noPrompts = false, QWidget * parent_ = 0, Qt::WindowFlags flags = 0);
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

  /** Sets the background color of the cell, depending on whether the update is made by read (normal color)
   *  or manually by the user (red).
   */


  /*
   * sort the register tree widget or 'unsort' it by rereading
   */
  void handleSortCheckboxClick(int state);

  /** Set all background to non modified color.
   */

  /*
   * returns true if the fileName ends with the provided
   * extension else false
   */
  bool checkExtension(QString const &fileName, QString extension);


  void copyRegisterTreeItemNameToClipBoard();

 private:
  Ui::QtHardMonForm ui; ///< The GUI form which hold all the widgets.
  mtca4u::Device currentDevice_; ///< The instance of the device which is being accessed.
  bool autoRead_; ///< Flag whether to automatically read on register change
  bool readOnClick_; ///< Flag wheter to read on click in the register list
  QString dmapFileName_; ///< The file name of the last opened dmap file
  QString configFileName_; ///< Name of the config file (last saved or read)
  int insideReadOrWrite_; ///< Counter flag to indicate if the read or write function is being executed
  bool noPrompts_;
  PropertiesWidgetProvider propertiesWidgetProvider_;
  /**
   *  Write the config to the given file name.
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

  /** A helper class to store listWidgetItems which also contain the dRegisterInfo and ptrmapFile information.
   */
  class DeviceListItem: public QListWidgetItem
  {
    public:
      /** The simplest cvonstructor, no text or icon for the entry*/
      DeviceListItem ( mtca4u::DeviceInfoMap::DeviceInfo const & device_map_emlement, QListWidget * parent_ = 0 );

      /** Constructor which sets the text entry in the list. */
      DeviceListItem ( mtca4u::DeviceInfoMap::DeviceInfo const & device_map_emlement, const QString & text_, QListWidget * parent_ = 0 );

      /** Constructor which sets the text entry in the list and an icon. */      
      DeviceListItem ( mtca4u::DeviceInfoMap::DeviceInfo const & device_map_emlement,
		       const QIcon & icon_, const QString & text_, QListWidget * parent_ = 0 );

      /* No copy constructor, the default is fine. */
      //DeviceListItem ( const DeviceListItem & other );

      /* No assignment operator, the default is fine. */
      //DeviceListItem & operator=( const DeviceListItem & other );
      
      /** The destructor. Currently does nothing because the members go out of scope automatically. */
      virtual ~DeviceListItem();
      
      /** Returns a reference to the deviceMapElement, i.e. the device information. */
      mtca4u::DeviceInfoMap::DeviceInfo const & getDeviceMapElement() const;
      
      /** The type of DeviceListItemType.  It's a user type because it is larger than QListWidgetItem::UserType.
       */
      static const int DeviceListItemType = QListWidgetItem::UserType + 1;

    private:
      mtca4u::DeviceInfoMap::DeviceInfo _deviceMapElement; ///< The instance of the DeviceMapElement
    public:
      std::vector<std::string> lastSelectedRegister_; ///< The last selected register before the item was deselected
  };

  DeviceListItem * _currentDeviceListItem; ///< Pointer to the currently selected deviceListItem

  PlotWindow * _plotWindow; ///< The plot window

  // The plot window is allowed to access the internal varaibles (mutex and register content) for plotting.
  // We allow this for performance reasons: A thread-safe getter for the data members would require one further
  // copying of the data, a plot function in the plot window class, which is called with a reference of the data 
  // pointer, would require unnecessarily long locking of the mutex.
  friend class PlotWindow;

  bool isMultiplexedDataRegion(std::string const & registerName);
  bool isSeqDescriptor(std::string const & registerName);

  std::string extractMultiplexedRegionName(std::string const & regName);

  std::string extractFileNameFromPath(const std::string &);
  private:

  /**
   *  Give Ctrl+c support for RegisterTreeWidget. Currently calls
   *  copyRegisterTreeItemNameToClipBoard
   */
  void addCopyActionForRegisterTreeWidget();
 /*
  *  populate the register list for the selected device in the GUI. Exits when
  *  deviceItem is NULL pointer
  */
  void populateRegisterTree(QListWidgetItem *deviceItem);

  // Disable copy constructor and assignment operator
  // This is the main class and it should'nt need copying
  // TODO: confirm this assumption
  Q_DISABLE_COPY(QtHardMon) // Easy way to get around -Weffc++ warning:
                            // class QtHardMon’ has pointer data members -
                            // but does not overide copy constructor and
                            // assignment operator

  void showMessageBox(QMessageBox::Icon boxType, QString boxTitle, 
                     QString boxText, QString boxInformativeText);

};



#endif// QT_HARD_MON
