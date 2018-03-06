#include "QtHardMon.h"
#include "ConfigFileReaderWriter.h"
#include "QtHardMonVersion.h"
#include "ui_PreferencesForm.h"

#include <iostream>
#include <limits>
#include <sstream>

#include <QDockWidget>
#include <QFileDialog>
#include <qaction.h>

#include <mtca4u/BackendFactory.h>
#include <mtca4u/DMapFileParser.h>
#include <mtca4u/Exception.h>
#include <mtca4u/PcieBackendException.h>

#include "Constants.h"
#include "Exceptions.h"
#include <QDebug>
#include <QTextStream>
using namespace mtca4u;

#include "DeviceElementQTreeItem.h"
#include "PreferencesProvider.h"
#include "RegisterTreeUtilities.h"

// Some variables to avoid duplication and possible inconsistencies in the code.
// These strings are used in the config file
#define DMAP_FILE_STRING "dmapFile"
#define CURRENT_DEVICE_STRING "currentDevice"
#define CURRENT_REGISTER_STRING "currentRegister"
#define CURRENT_MODULE_STRING "currentModule"
#define MAX_WORDS_STRING "maxWords"
#define PRECISION_INDICATOR_STRING "decimalPlaces"
#define READ_AFTER_WRITE_STRING "readAfterWrite"
#define AUTOSELECT_PREVIOUS_REGISTER_STRING "autoselectPreviousRegister"
#define SHOW_PLOT_WINDOW_STRING "showPlotWindow"
#define PLOT_AFTER_READ_STRING "plotAfterRead"
#define REGISTER_EXTENSION_STRING "_REGISTER"
#define MODULE_EXTENSION_STRING "_MODULE"
#define FONT_SIZE_STRING "fontSize"
#define AUTO_READ_STRING "autoRead"
#define READ_ON_CLICK_STRING "readOnClick"
#define NO_MODULE_NAME_STRING "[No Module Name]"

QtHardMon::QtHardMon(bool noPrompts, QWidget *parent_, Qt::WindowFlags flags)
    : QMainWindow(parent_, flags), ui(), dmapFileName_(), configFileName_(),
      insideReadOrWrite_(0), _currentDeviceListItem(NULL), _plotWindow(NULL) {

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();
  ui.setupUi(this);

  preferencesProvider.setValue("noPrompts", noPrompts);
  preferencesProvider.setValue("readOnClick", true);
  preferencesProvider.setValue("autoRead", true);
  preferencesProvider.setValue("maxWords", 0x10000);
  preferencesProvider.setValue(
      "floatPrecision",
      static_cast<int>(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION));

  setWindowTitle("QtHardMon");
  setWindowIcon(QIcon(":/DESY_logo_nofade.png"));
  ui.logoLabel->setPixmap(QPixmap(":/DESY_logo.png"));

  /// @todo FIXME Re-activate this functionality
  //  addCopyActionForRegisterTreeWidget(); // Adds slot to copy qtreeiem's name to
                                        // clipboard

  connect(ui.deviceListWidget,
          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
          this, SLOT(deviceSelected(QListWidgetItem *, QListWidgetItem *)));

  connect(ui.registerTreeWidget,
          SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
          this, SLOT(registerSelected(QTreeWidgetItem *, QTreeWidgetItem *)));

  connect(ui.registerTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
          this, SLOT(registerClicked(QTreeWidgetItem *)));

  connect(ui.loadBoardsButton, SIGNAL(clicked()), this, SLOT(loadBoards()));

  connect(ui.readButton, SIGNAL(clicked()), this, SLOT(read()));

  connect(ui.writeButton, SIGNAL(clicked()), this, SLOT(write()));

  connect(ui.aboutQtHardMonAction, SIGNAL(triggered()), this,
          SLOT(aboutQtHardMon()));

  connect(ui.aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));

  connect(ui.preferencesAction, SIGNAL(triggered()), this, SLOT(preferences()));

  connect(ui.loadConfigAction, SIGNAL(triggered()), this, SLOT(loadConfig()));

  connect(ui.saveConfigAction, SIGNAL(triggered()), this, SLOT(saveConfig()));

  connect(ui.saveConfigAsAction, SIGNAL(triggered()), this,
          SLOT(saveConfigAs()));

  connect(ui.loadBoardsAction, SIGNAL(triggered()), this, SLOT(loadBoards()));

  connect(ui.quitAction, SIGNAL(triggered()), this, SLOT(close()));

  connect(ui.openCloseButton, SIGNAL(clicked()), this, SLOT(openCloseDevice()));

  connect(ui.SortAscendingcheckBox, SIGNAL(stateChanged(int)), this,
          SLOT(handleSortCheckboxClick(int)));

  // The oparations and options group are disabled until a dmap file is loaded
  // and a device has been opened
  ui.operationsGroupBox->setEnabled(false);
  ui.optionsGroupBox->setEnabled(false);
  ui.deviceStatusGroupBox->setEnabled(false);
  ui.devicePropertiesGroupBox->setEnabled(false);

  // The following widgets are diabled because they are not implemented yet
  ui.continuousReadCheckBox->setEnabled(false);
  ui.writeToFileButton->setEnabled(false);
  ui.readFromFileButton->setEnabled(false);

  _plotWindow = new PlotWindow(this);

  // sorted enabled by default; calling this here will trigger the slot, which
  // sorts the register list.
  ui.SortAscendingcheckBox->setChecked(true);

  connect(ui.showPlotWindowCheckBox, SIGNAL(stateChanged(int)), this,
          SLOT(showPlotWindow(int)));

  connect(_plotWindow, SIGNAL(plotWindowClosed()), this,
          SLOT(unckeckShowPlotWindow()));

  // also the plot window dfunctions are only enabled when a device is opened.
  _plotWindow->setEnabled(false);
}

QtHardMon::~QtHardMon() {}

void QtHardMon::loadBoards() {
  // Show a file dialog to select the dmap file.
  QString dmapFileName = QFileDialog::getOpenFileName(
      this, tr("Open DeviceMap file"), ".",
      tr("DeviceMap files (*.dmap);; All files (*)"));

  if (dmapFileName.isEmpty()) {
    // No file name selected, just quit.
    return;
  }

  // set the directory of the dmap file as the current working directory so
  // relative map file pathes work
  // (I use c-type because it's shorter syntax and does not matter here).
  QDir::setCurrent(QFileInfo(dmapFileName).absolutePath());

  // The return value is intentionally ignored. Cast to void to suppress
  // compiler warnings.
  // (I use c-type because it's shorter syntax and does not matter here).
  (void)loadDmapFile(dmapFileName);
}

bool QtHardMon::loadDmapFile(QString const &dmapFileName) {
  mtca4u::DMapFileParser fileParser;
  DeviceInfoMapPointer dmap;
  try {
    dmap = fileParser.parse(dmapFileName.toStdString());
    BackendFactory::getInstance().setDMapFilePath(dmapFileName.toStdString());
  } catch (Exception &e) {
    showMessageBox(
        QMessageBox::Critical, QString("QtHardMon : Error"),
        QString("Could not load DeviceMap file " + dmapFileName + "."),
        QString("Info: An exception was thrown:") + e.what());

    // We just return after displaying the message and leave the deviceList as
    // it was.
    return false;
  }

  // store the dmap file name for further usage. The variable with the
  // underscore is the class wide variable.
  dmapFileName_ = dmapFileName;

  // clear the device list and the device specific info
  ui.deviceListWidget->clear();

  // Set the keyboard focus away from the deviceListWidget. This would trigger
  // the deviceSelected
  // on the first entry, which we don't want. The focus is set to the
  // registerTreeWidget.
  ui.registerTreeWidget->setFocus(Qt::OtherFocusReason);

  for (DeviceInfoMap::iterator deviceIter = dmap->begin();
       deviceIter != dmap->end(); ++deviceIter) {
    ui.deviceListWidget->addItem(new DeviceListItem(
        (*deviceIter), (*deviceIter).deviceName.c_str(), ui.deviceListWidget));
  }
  return true;
  // on user request: do not automatically load the first device. This might be
  // not accessible and
  // immediately gives an error message.
  // ui.deviceListWidget->setCurrentRow(0);
}

void QtHardMon::deviceSelected(QListWidgetItem *deviceItem,
                               QListWidgetItem * /*previousDeviceItem */) {
  ui.devicePropertiesGroupBox->setEnabled(true);

  // When the deviceListWidget is cleared , the currentItemChanged signal is
  // emitted with a null pointer. We have to catch this here and return. Before
  // returning we clear the device specific display info, close the device and
  // empty the register list.
  if (!deviceItem) {
    ui.deviceNameDisplay->setText("");
    ui.deviceFileDisplay->setText("");
    ui.mapFileDisplay->setText("");
    ui.mapFileDisplay->setToolTip("");
    ui.deviceStatusGroupBox->setEnabled(false);
    ui.devicePropertiesGroupBox->setEnabled(false);
    closeDevice();
    ui.registerTreeWidget->clear();

    return;
  }

  ui.deviceStatusGroupBox->setEnabled(true);
  ui.devicePropertiesGroupBox->setEnabled(true);

  DeviceListItem *deviceListItem = static_cast<DeviceListItem *>(deviceItem);

  _currentDeviceListItem = deviceListItem;
  // close the previous device. This also disables the relevant GUI elements
  closeDevice();
  // opening the device enables the gui elements if success
  openDevice(deviceListItem->getDeviceMapElement().deviceName);

  ui.deviceNameDisplay->setText(
      deviceListItem->getDeviceMapElement().deviceName.c_str());
  ui.deviceFileDisplay->setText(
      deviceListItem->getDeviceMapElement().uri.c_str());

  std::string absPath = deviceListItem->getDeviceMapElement().mapFileName;
  std::string mapFileName = extractFileNameFromPath(absPath);
  ui.mapFileDisplay->setText(mapFileName.c_str());
  ui.mapFileDisplay->setToolTip(absPath.c_str());
  try {
    populateRegisterTree(deviceItem);
  } catch (Exception &e) {
    // In case anything fails, we would like to catch it and close the device.
	///@todo FIXME catch exceptions for all registers and show the message only once.
	std::cout<<"This exception area is still under construction:"<<e.what()<<std::endl;
    //closeDevice();
    //ui.registerTreeWidget->clear();
  }
}

void QtHardMon::openDevice(
    std::string const &deviceFileName) // Change name to createAndOpenDevice();
{
  // try to open a createa and new device. If this fails disable the buttons and
  // the registerValues
  try {
    currentDevice_.open(deviceFileName);
    // enable all of the GUI in case it was deactivated before
    ui.propertiesWidget->ui.valuesTableWidget->setEnabled(true);
    ui.operationsGroupBox->setEnabled(true);
    ui.optionsGroupBox->setEnabled(true);
    _plotWindow->setEnabled(true);

    ui.openClosedLabel->setText( "Device is open.");
    ui.openCloseButton->setText("Close");
  } catch (Exception &e) {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
                   QString("Could not create the device ") +
                       deviceFileName.c_str() + ".",
                   QString("Info: An exception was thrown:") + e.what());
  }
}

void QtHardMon::closeDevice() {
  if (currentDevice_.isOpened())
    currentDevice_.close();
  ui.propertiesWidget->ui.valuesTableWidget->setEnabled(false);
  ui.operationsGroupBox->setEnabled(false);
  ui.optionsGroupBox->setEnabled(false);
  _plotWindow->setEnabled(false);
  // If the device is closed then there is no way we can read values from the
  // registers - they are not available anymore. Nothing to show on the table
  ui.propertiesWidget->clearFields();
  ui.openClosedLabel->setText("Device is closed.");
  ui.openCloseButton->setText("Open");
}

void QtHardMon::registerSelected(QTreeWidgetItem *registerItem,
                                 QTreeWidgetItem * /*previousRegisterItem */) {
  // There is a case when a device entry is clicked in the device list, the slot
  // is called with a NULL registerItem
  if (!registerItem) {
    ui.propertiesWidget->clearFields();
    return;
  }

  // we know that the registerItem is a DeviceElementQTreeItem, so we can static cast.
  DeviceElementQTreeItem *selectedItem =
      static_cast<DeviceElementQTreeItem *>(registerItem);
  ui.propertiesWidget->updateRegisterInfo(selectedItem->getRegisterInfo());

  _currentDeviceListItem->lastSelectedRegister_.clear();
  while (selectedItem && !dynamic_cast<QTreeWidget *>(selectedItem)) {
    _currentDeviceListItem->lastSelectedRegister_.insert(
        _currentDeviceListItem->lastSelectedRegister_.begin(),
        selectedItem->text(0).toStdString());
    selectedItem =
        dynamic_cast<DeviceElementQTreeItem *>(selectedItem->parent());
  }

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  if (!preferencesProvider.getValue<bool>("autoRead")) {
    // If automatic reading is deactivated the widget has to be cleared so all
    // widget items are empty.
    // In addition the write button is deactivated so the invalid items cannot
    // be written to the register.
    ui.propertiesWidget->ui.valuesTableWidget->clearContents();
    ui.propertiesWidget->ui.valuesTableWidget->setRowCount(0);
    ui.writeButton->setEnabled(false);
  } else {
    read(true);
  }
}

void QtHardMon::read(bool autoRead) {
  ++insideReadOrWrite_;
  DeviceElementQTreeItem *registerTreeItem =
      static_cast<DeviceElementQTreeItem *>(
          ui.registerTreeWidget->currentItem());
  if (!registerTreeItem)
  {
	  showMessageBox(QMessageBox::Information, QString("QtHardMon : Information"),
	                     QString("No register selected"),QString("Please select a valid register"));
	  return;
  }
  try {
    if (currentDevice_.isOpened()) {
      ///@todo FIXME I disabled reading when opening a devide. This should not happen anyway.
      //registerTreeItem->readData(currentDevice_);
      ui.writeButton->setEnabled(true);
    }
  } catch (InvalidOperationException &e) {

    if (!autoRead) {
      showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
                     QString("QtHardMon read error "),
                     QString("Info: An exception was thrown:") + e.what());
    }
  } catch (std::exception &e) {

    if (!autoRead) {
      closeDevice();
      ui.writeButton->setEnabled(false);
      // the error message accesses the _currentDeviceListItem. Is
      // this safe? It might be NULL.
      showMessageBox(
          QMessageBox::Critical, QString("QtHardMon : Error"),
          QString("Error reading from device ") +
              _currentDeviceListItem->getDeviceMapElement().uri.c_str() + ".",
          QString("Info: An exception was thrown:") + e.what() +
              QString("\n\nThe device has been closed."));
    }
  }

  // check if plotting after reading is requested
  if (_plotWindow->isVisible() && _plotWindow->plotAfterReadIsChecked()) {
    _plotWindow->plot();
  }
  --insideReadOrWrite_;

}

void QtHardMon::write() {
  ++insideReadOrWrite_;
  DeviceElementQTreeItem *registerTreeItem =
      static_cast<DeviceElementQTreeItem *>(
          ui.registerTreeWidget->currentItem());

  try {
    if (currentDevice_.isOpened()) {
      ///@todo FIXME Writing mechanism has to change. Not implemented
      //registerTreeItem->writeData(currentDevice_);
    }
  } catch (InvalidOperationException &e) {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
                   QString("QtHardMon write error "),
                   QString("Info: An exception was thrown:") + e.what());
    return;
  } catch (std::exception &e) {
    closeDevice();

    // the error message accesses the _currentDeviceListItem. Is this safe? It
    // might be NULL.
    showMessageBox(
        QMessageBox::Critical, QString("QtHardMon : Error"),
        QString("Error writing to device ") +
            _currentDeviceListItem->getDeviceMapElement().uri.c_str() + ".",
        QString("Info: An exception was thrown:") + e.what() +
            QString("\n\nThe device has been closed."));
  }

  if (ui.readAfterWriteCheckBox->isChecked()) {
    read();
  } else {
    ui.propertiesWidget->clearDataWidgetBackground();
  }

  --insideReadOrWrite_;
}

void QtHardMon::preferences() {
  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  // create a preferences dialog and set the correct warning message with
  // contains the default number for maxWords.
  QDialog preferencesDialog;
  Ui::PreferencesDialogForm preferencesDialogForm;
  preferencesDialogForm.setupUi(&preferencesDialog);
  preferencesDialogForm.maxWordsWarningLabel->setText(
      QString("WARNING:") +
      " Setting this value too high can exhaust your memory, " +
      "which will lead to a segmentation fault. Default value is " +
      QString::number(0x10000));

  preferencesDialogForm.fontSizeSpinBox->setValue(font().pointSize());
  preferencesDialogForm.autoReadCheckBox->setChecked(
      preferencesProvider.getValue<bool>("autoRead"));
  preferencesDialogForm.readOnClickCheckBox->setChecked(
      preferencesProvider.getValue<bool>("readOnClick"));

  // set up the current value of maxWords
  preferencesDialogForm.maxWordsSpinBox->setMaximum(INT_MAX);
  preferencesDialogForm.maxWordsSpinBox->setValue(
      preferencesProvider.getValue<int>("maxWords"));

  // set up the floating point display decimal places
  preferencesDialogForm.precisionSpinBox->setMinimum(
      1); // minimum one decimal place display
  preferencesDialogForm.precisionSpinBox->setMaximum(
      10); // maximum 10 decimal places
  preferencesDialogForm.precisionSpinBox->setValue(
      preferencesProvider.getValue<int>("floatPrecision"));

  int dialogResult = preferencesDialog.exec();

  // only set the values if ok has been pressed
  if (dialogResult == QDialog::Accepted) {
    preferencesProvider.setValue(
        "maxWords", preferencesDialogForm.maxWordsSpinBox->value());

    // Read and set precision for delegate class
    preferencesProvider.setValue(
        "floatPrecision", preferencesDialogForm.precisionSpinBox->value());

    //@todo FIXME: There is no custom delegate at the moment.
    //@todo FIXME: this should be done on local level of properties widgets.
    //ui.propertiesWidget->customDelegate_.setDoubleSpinBoxPrecision(
    //preferencesProvider.getValue<int>("floatPrecision"));

    // call registerSelected() so the size of the valuesList is adapted and
    // possible missing values are read
    // from the device
    registerSelected(ui.registerTreeWidget->currentItem(),
                     ui.registerTreeWidget->currentItem());

    QFont newFont(this->font());
    newFont.setPointSize(preferencesDialogForm.fontSizeSpinBox->value());
    QApplication::setFont(newFont);

    preferencesProvider.setValue(
        "readOnClick", preferencesDialogForm.readOnClickCheckBox->isChecked());
    preferencesProvider.setValue(
        "autoRead", preferencesDialogForm.autoReadCheckBox->isChecked());
  }
}

void QtHardMon::aboutQtHardMon() {
  QMessageBox::about(this, "About QtHardMon",
                     QString("QtHardMon Version ") + QT_HARD_MON_VERSION);
}

void QtHardMon::aboutQt() { QMessageBox::aboutQt(this, "About Qt"); }

void QtHardMon::loadConfig() {
  // the local, temporary variable, without underscore
  QString configFileName = QFileDialog::getOpenFileName(
      this, tr("Open DeviceMap file"), ".",
      tr("HardMon config files (*.cfg);; All files (*)"));

  if (configFileName.isEmpty()) {
    // No file name selected, just quit.
    return;
  }

  loadConfig(configFileName);
}

void QtHardMon::loadConfig(QString const &configFileName) {

  ConfigFileReaderWriter configReader;
  try {
    // even the constructor can throw when opening and reading a file
    configReader.read(configFileName.toStdString());
  } catch (std::ifstream::failure &e) {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
                   QString("Could not read config file ") + configFileName +
                       ".",
                   QString("Info: An exception was thrown:") + e.what());
    return;
  }

  // At this point we considder the config file as successfully loaded, even
  // though it might not contain any
  // valid parameters. But we store the file name.
  configFileName_ = configFileName;

  // show message box with parse errors, but just continue normally
  if (!configReader.getBadLines().isEmpty()) {
    QString infoText;
    for (QStringList::const_iterator badLinesIter =
             configReader.getBadLines().begin();
         badLinesIter != configReader.getBadLines().end(); ++badLinesIter) {
      infoText += *badLinesIter;
      infoText += "\n";
    }

    showMessageBox(
        QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString("The following lines from the config file ") + configFileName +
            " are invalid and will be ignored. Please fix your config file.\n",
        infoText);
    return;
  }

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  // first handle all settings that do not depend on opening a device map

  preferencesProvider.setValue(
      "floatPrecision",
      configReader.getValue(
          PRECISION_INDICATOR_STRING,
          preferencesProvider.getValue<int>("floatPrecision")));

  // store in a local variable for now
  int maxWords = configReader.getValue(
      MAX_WORDS_STRING, preferencesProvider.getValue<int>("maxWords"));
  // check for validity. Minimum reasonable value is 1.
  if (maxWords >= 1) {
    // only after checking set the class wide maxWords variable
    preferencesProvider.setValue("maxWords", maxWords);
    // Update the register, so the length of the valuesList is adapted.
    // If another register is loaded from the config this might be repeated.
    // But for an easier logic we take this little overhead.
    registerSelected(ui.registerTreeWidget->currentItem(),
                     ui.registerTreeWidget->currentItem());
  } else {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
                   QString("Read invalid maxWords from config file."),
                   QString(""));
  }

  int readAfterWriteFlag = configReader.getValue(
      READ_AFTER_WRITE_STRING, ui.readAfterWriteCheckBox->isChecked() ? 1 : 0);
  // we use the implicit conversion 0=false, everyting else is true
  ui.readAfterWriteCheckBox->setChecked(readAfterWriteFlag);

  int showPlotWindowFlag = configReader.getValue(
      SHOW_PLOT_WINDOW_STRING, ui.showPlotWindowCheckBox->isChecked() ? 1 : 0);
  ui.showPlotWindowCheckBox->setChecked(showPlotWindowFlag);

  int plotAfterReadFlag = configReader.getValue(
      PLOT_AFTER_READ_STRING, _plotWindow->plotAfterReadIsChecked() ? 1 : 0);
  _plotWindow->setPlotAfterRead(plotAfterReadFlag);

  int autoselectPreviousRegisterFlag = configReader.getValue(
      AUTOSELECT_PREVIOUS_REGISTER_STRING,
      ui.autoselectPreviousRegisterCheckBox->isChecked() ? 1 : 0);
  ui.autoselectPreviousRegisterCheckBox->setChecked(
      autoselectPreviousRegisterFlag);

  preferencesProvider.setValue(
      "readOnClick",
      static_cast<bool>(configReader.getValue(
          READ_ON_CLICK_STRING,
          preferencesProvider.getValue<bool>("readOnClick") ? 1 : 0)));
  preferencesProvider.setValue(
      "autoRead", static_cast<bool>(configReader.getValue(
                      AUTO_READ_STRING,
                      preferencesProvider.getValue<bool>("autoRead") ? 1 : 0)));

  int fontSize = configReader.getValue(FONT_SIZE_STRING, font().pointSize());
  // Check validity of the font size.
  if (fontSize < 1 || fontSize > 99) {
  } else {
    QFont newFont(this->font());
    newFont.setPointSize(fontSize);
    QApplication::setFont(newFont);
  }

  // now read the mapping file, device and register. If anything goes wrong we
  // can just exit the function because
  // all other variables have already been processed.
  std::string dmapFileString =
      configReader.getValue(DMAP_FILE_STRING, std::string());

  if (dmapFileString.empty()) {
    // no dmap file. Just return, also loading device and register does not make
    // sense without dmap
    return;
  }

  if (!loadDmapFile(dmapFileString.c_str())) {
    // Just return, also loading device and register does not make sense without
    // dmap
    return;
  }

  // loop all devices and try to determine the last used module and register
  for (int deviceRow = 0; deviceRow < ui.deviceListWidget->count();
       ++deviceRow) {
    DeviceListItem *deviceListItem =
        static_cast<DeviceListItem *>(ui.deviceListWidget->item(deviceRow));

    // determine the module and the register
    std::string deviceRegisterString =
        deviceListItem->getDeviceMapElement().deviceName +
        REGISTER_EXTENSION_STRING;
    std::string registerName =
        configReader.getValue(deviceRegisterString, std::string());

    std::string deviceModuleString =
        deviceListItem->getDeviceMapElement().deviceName +
        MODULE_EXTENSION_STRING;
    std::string moduleName =
        configReader.getValue(deviceModuleString, std::string());

    // deviceListItem->setLastSelectedRegisterName(registerName);
    // deviceListItem->setLastSelectedModuleName(moduleName);
  } // for deviceRow

  // search for the device string
  std::string currentDeviceString =
      configReader.getValue(CURRENT_DEVICE_STRING, std::string());

  if (currentDeviceString.empty()) {
    // no device specified, noting left to do
    return;
  }

  // try to find the device in the list
  QList<QListWidgetItem *> matchingDevices = ui.deviceListWidget->findItems(
      currentDeviceString.c_str(), Qt::MatchExactly);

  if (matchingDevices.isEmpty()) {
    // the item should have been there, give a warning
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
                   QString("Device ") + currentDeviceString.c_str() +
                       " is not in the dmap file.",
                   QString(""));
    return;
  }

  // It's safe to call static_cast because the deviceListWidget is controlled by
  // this application.
  // We know that it is a deviceListItem, no nynamic checking needed.
  // We also know that matchingDevices.begin() is valid because the list is not
  // empty.
  // In case there is more than one entry with the same name we just pick the
  // first one.
  DeviceListItem *deviceListItem =
      static_cast<DeviceListItem *>(*matchingDevices.begin());

  // now we are ready to select the device
  ui.deviceListWidget->setCurrentItem(deviceListItem);
}

void QtHardMon::saveConfig() {
  if (configFileName_.isEmpty()) {
    saveConfigAs();
  } else {
    writeConfig(configFileName_);
  }
}

void QtHardMon::saveConfigAs() {
  // the local, temporary variable, without underscore
  QString configFileName = QFileDialog::getSaveFileName(
      this, tr("Open DeviceMap file"), ".",
      tr("HardMon config files (*.cfg);; All files (*)"));

  if (configFileName.isEmpty()) {
    // No file name selected, just quit.
    return;
  }

  // The file name seems valid. Now try to write it and store the name in the
  // class wide variable if successful.
  try {
    writeConfig(configFileName);
  } catch (std::ifstream::failure &e) {
    // a message box with the error warning has already been displayed. Nothing
    // more to do but quit.
    return;
  }

  // only store the file name upon successful writing (to the class wide
  // variable with the unterscore)
  configFileName_ = configFileName;
}

void QtHardMon::writeConfig(QString const &fileName) {
  ConfigFileReaderWriter configWriter;

  // set the variables we want do write to file

  // only write the file name when it's not empty, otherwise we get an invalid
  // line
  if (!dmapFileName_.isEmpty()) {
    configWriter.setValue(DMAP_FILE_STRING, dmapFileName_.toStdString());
  }

  // The device list widget only contains deviceListItems, so it's safe to use a
  // static cast here.
  DeviceListItem *deviceListItem =
      static_cast<DeviceListItem *>(ui.deviceListWidget->currentItem());

  // the device list might be emtpy, or there is no current item (?, is this
  // possibe?)

  if (deviceListItem) {
    configWriter.setValue(CURRENT_DEVICE_STRING,
                          deviceListItem->getDeviceMapElement().deviceName);
    // writing a register without item does not make sense, so we keep it in the
    // if block
    //    configWriter.setValue(CURRENT_REGISTER_ROW_STRING,
    //    ui.registerTreeWidget->currentRow());
  }

  // add a value to store the last register for each device
  for (int deviceRow = 0; deviceRow < ui.deviceListWidget->count();
       ++deviceRow) {
    deviceListItem =
        static_cast<DeviceListItem *>(ui.deviceListWidget->item(deviceRow));

    // Only write to the config file if the 'last selected' strings are not
    // empty.
    // Empty strings would cause a parse error, and if the entry is not found it
    // falls back to empty string anyway.
    if (!deviceListItem->lastSelectedRegister_.empty()) {
      std::string deviceRegisterString =
          deviceListItem->getDeviceMapElement().deviceName +
          REGISTER_EXTENSION_STRING;
      configWriter.setValue(deviceRegisterString,
                            deviceListItem->lastSelectedRegister_.back());
    }

    if (!deviceListItem->lastSelectedRegister_.empty()) {
      std::string deviceModuleString =
          deviceListItem->getDeviceMapElement().deviceName +
          MODULE_EXTENSION_STRING;
      configWriter.setValue(deviceModuleString,
                            deviceListItem->lastSelectedRegister_.front());
    }
  }

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  configWriter.setValue(MAX_WORDS_STRING,
                        preferencesProvider.getValue<int>("maxWords"));
  configWriter.setValue(PRECISION_INDICATOR_STRING,
                        preferencesProvider.getValue<int>("floatPrecision"));
  configWriter.setValue(READ_AFTER_WRITE_STRING,
                        ui.readAfterWriteCheckBox->isChecked() ? 1 : 0);
  configWriter.setValue(SHOW_PLOT_WINDOW_STRING,
                        ui.showPlotWindowCheckBox->isChecked() ? 1 : 0);
  configWriter.setValue(AUTOSELECT_PREVIOUS_REGISTER_STRING,
                        ui.autoselectPreviousRegisterCheckBox->isChecked() ? 1
                                                                           : 0);
  configWriter.setValue(PLOT_AFTER_READ_STRING,
                        _plotWindow->plotAfterReadIsChecked() ? 1 : 0);
  configWriter.setValue(FONT_SIZE_STRING, font().pointSize());
  configWriter.setValue(AUTO_READ_STRING,
                        preferencesProvider.getValue<bool>("autoRead") ? 1 : 0);
  configWriter.setValue(READ_ON_CLICK_STRING,
                        preferencesProvider.getValue<bool>("readOnClick") ? 1
                                                                          : 0);

  // this
  try {
    configWriter.write(fileName.toStdString());
  }
  // we catch a write failure here to show a message box, but rethrow the
  // exception
  catch (std::ifstream::failure &e) {
    showMessageBox(QMessageBox::Critical, QString("QtHardMon : Error"),
                   QString("Could not write config file ") + fileName + ".",
                   QString("Info: An exception was thrown:") + e.what());

    // rethrow the exception so the calling code knows that writing failed.
    throw;
  }
}

void QtHardMon::showPlotWindow(int checkState) {
  if (checkState == Qt::Unchecked) {
    _plotWindow->setVisible(false);
  } else {
    _plotWindow->plot();
    _plotWindow->setVisible(true);
  }
}

void QtHardMon::unckeckShowPlotWindow() {
  // the plot window just closed. Uncheck the showPlotWindow check box
  ui.showPlotWindowCheckBox->setChecked(false);
}

// The constructor itself is empty. It just calls the construtor of the mother
// class and the copy
// constructors of the data members
QtHardMon::DeviceListItem::DeviceListItem(
    mtca4u::DeviceInfoMap::DeviceInfo const &device_map_emlement,
    QListWidget *parent_)
    : QListWidgetItem(parent_, DeviceListItemType),
      _deviceMapElement(device_map_emlement)

{}

QtHardMon::DeviceListItem::DeviceListItem(
    mtca4u::DeviceInfoMap::DeviceInfo const &device_map_emlement,
    const QString &text_, QListWidget *parent_)
    : QListWidgetItem(text_, parent_, DeviceListItemType),
      _deviceMapElement(device_map_emlement) {}

QtHardMon::DeviceListItem::DeviceListItem(
    mtca4u::DeviceInfoMap::DeviceInfo const &device_map_emlement,
    const QIcon &icon_, const QString &text_, QListWidget *parent_)
    : QListWidgetItem(icon_, text_, parent_, DeviceListItemType),
      _deviceMapElement(device_map_emlement) {}

QtHardMon::DeviceListItem::~DeviceListItem() {}

mtca4u::DeviceInfoMap::DeviceInfo const &
QtHardMon::DeviceListItem::getDeviceMapElement() const {
  return _deviceMapElement;
}

void QtHardMon::registerClicked(QTreeWidgetItem * /*registerItem*/) {
  // Do not execute the read if the corresponding flag is off
  // registerSelected method.

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  if (preferencesProvider.getValue<bool>("readOnClick")) {
    //    std::cout << "Ignoring click" <<std::endl;
    return;
  }

  read();
}

void QtHardMon::openCloseDevice() {
  if (currentDevice_.isOpened()) {
    closeDevice();
  } else {
    openDevice(_currentDeviceListItem->getDeviceMapElement().deviceName);
  }
}

void QtHardMon::parseArgument(QString const &fileName) {
  if (checkExtension(fileName, ".dmap") == true) {
    (void)loadDmapFile(fileName);
  } else if (checkExtension(fileName, ".cfg") == true) {
    loadConfig(fileName);
  } else {
    showMessageBox(
        QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString(
            "Unsupported file extension provided. Filename will be ignored."),
        QString(""));
  }
}

bool QtHardMon::checkExtension(QString const &fileName, QString extension) {
  QStringRef extensionOfProvidedFile(
      &fileName, (fileName.size() - extension.size()), extension.size());
  bool areStringsEqual = (extension.compare(extensionOfProvidedFile) == 0);
  return areStringsEqual;
}

bool QtHardMon::isMultiplexedDataRegion(const std::string &registerName) {
  if (registerName.substr(0,
                          std::string("AREA_MULTIPLEXED_SEQUENCE_").size()) ==
      "AREA_MULTIPLEXED_SEQUENCE_") {
    return true;
  }
  return false;
}

std::string QtHardMon::extractFileNameFromPath(const std::string &fileName) {
  std::string extractedName = fileName;
  size_t position = fileName.find_last_of('/');
  if (position != std::string::npos) {
    extractedName = fileName.substr(position + 1, std::string::npos);
  }
  return extractedName;
}

void QtHardMon::populateRegisterTree(QListWidgetItem *deviceItem) {

  if (deviceItem == NULL) {
    return;
  }

  // the deviceItem actually is a DeviceListItemType. As this is a private slot
  // it is safe to assume this and use a static cast.
  DeviceListItem *deviceListItem = static_cast<DeviceListItem *>(deviceItem);

  ui.registerTreeWidget->clear();

  const mtca4u::RegisterCatalogue registerCatalogue =
      currentDevice_.getRegisterCatalogue();
  // get the registerMap and fill the RegisterTreeWidget
  for (RegisterCatalogue::const_iterator registerIter = currentDevice_.getRegisterCatalogue().begin();
       registerIter != currentDevice_.getRegisterCatalogue().end();
       ++registerIter) {
    // The QTreeItems are assigned with `unused` attribute, as they are
    // automatically appended to the tree structure.
    mtca4u::RegisterInfoMap::RegisterInfo *numericAddressedRegisterInfo =
        dynamic_cast<mtca4u::RegisterInfoMap::RegisterInfo *>(
            registerCatalogue.getRegister(registerIter->getRegisterName())
                .get());

    // parentNode can be null if there is no parent, i.e. the register is directly in the treeWidget
    auto parentNode = RegisterTreeUtilities::getDeepestBranchNode(registerCatalogue.getRegister(registerIter->getRegisterName()),ui.registerTreeWidget);
    if (parentNode){
      new DeviceElementQTreeItem(parentNode,
                                 registerIter->getRegisterName().getComponents().back().c_str(),
                                 registerCatalogue.getRegister(registerIter->getRegisterName()));
    }else{
      // no parent node. Directly add to the tree widget.
      new DeviceElementQTreeItem(ui.registerTreeWidget,
                                 registerIter->getRegisterName().getComponents().back().c_str(),
                                 registerCatalogue.getRegister(registerIter->getRegisterName()));      
    }
  }
  ui.registerTreeWidget->expandAll();
  if (ui.autoselectPreviousRegisterCheckBox->isChecked()) {
    // Searching a sub-tree does not work in QTreeWidget. So here is the
    // strategy:
    if (!(deviceListItem->lastSelectedRegister_.empty())) {
      // Get a list of all registers with this name.
      QList<QTreeWidgetItem *> registerList = ui.registerTreeWidget->findItems(
          deviceListItem->lastSelectedRegister_.back().c_str(),
          Qt::MatchExactly | Qt::MatchRecursive);

      deviceListItem->lastSelectedRegister_.pop_back();

      // Iterate the list until we find the one with the right module
      for (QList<QTreeWidgetItem *>::iterator registerIter =
               registerList.begin();
           registerIter != registerList.end(); ++registerIter) {
        std::vector<std::string> copyOfSelectedRegister =
            deviceListItem->lastSelectedRegister_;
        QTreeWidgetItem *temp = (*registerIter);
        // Since we might have selected a module (one item in the vector), we
        // assume initially, that we have found our selection - but loop might
        // change that.
        bool found = true;
        while (!(copyOfSelectedRegister.empty())) {
          QTreeWidgetItem *parentCast =
              dynamic_cast<QTreeWidgetItem *>(temp->parent());
          if (parentCast) {
            copyOfSelectedRegister.pop_back();
            temp = parentCast;
          } else {
            // That's not the one, move on
            found = false;
            break;
          }
          if (found) {
            ui.registerTreeWidget->setCurrentItem((*registerIter));
            break;
          }
        }
      }
    }
  }
  if (ui.registerTreeWidget->currentItem() == 0) // if no item is selected
  { //select first item, if list is not empty.
    QTreeWidgetItemIterator it(ui.registerTreeWidget);
    if (*it)
      ui.registerTreeWidget->setCurrentItem(*it);
  }
}

void QtHardMon::addCopyActionForRegisterTreeWidget() {
  QAction *copy = new QAction(tr("&Copy"), ui.registerTreeWidget);
  copy->setShortcuts(QKeySequence::Copy);
  copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(copy, SIGNAL(triggered()), this,
          SLOT(copyRegisterTreeItemNameToClipBoard()));
  ui.registerTreeWidget->addAction(copy);
}

void QtHardMon::copyRegisterTreeItemNameToClipBoard() {
  QTreeWidgetItem *currentItem = ui.registerTreeWidget->currentItem();
  if (currentItem) {
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->clear(QClipboard::Clipboard);
    // clear the 'selection clipboard'; basically content that you get on
    // center mouse click
    clipboard->clear(QClipboard::Selection);

    QString name = currentItem->text(0);
    clipboard->setText(name);
  }
  return;
}

void QtHardMon::handleSortCheckboxClick(int state) {
  if (state == Qt::Checked) {
    ui.registerTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    ui.registerTreeWidget->setSortingEnabled(true);
  } else if (state == Qt::Unchecked) {
    ui.registerTreeWidget->setSortingEnabled(false);
    // redraw the currently sorted tree to pick up the order from the mapfile.
    populateRegisterTree(ui.deviceListWidget->currentItem());
  }
}

void QtHardMon::showMessageBox(QMessageBox::Icon boxType, QString boxTitle,
                               QString boxText, QString boxInformativeText) {
  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  if (preferencesProvider.getValue<bool>("noPrompts")) {
    std::cout << boxText.toStdString() << " "
              << boxInformativeText.toStdString() << std::endl;
  } else {
    QMessageBox messageBox(boxType, boxTitle, boxText, QMessageBox::Ok, this);
    messageBox.setInformativeText(boxInformativeText);
    messageBox.exec();
  }
}
