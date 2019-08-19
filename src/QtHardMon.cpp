#include "QtHardMon.h"
#include "ConfigFileReaderWriter.h"
#include "QtHardMonVersion.h"
#include "ui_PreferencesForm.h"
#include <QHostInfo>

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

#include <QDockWidget>
#include <QFileDialog>
#include <qaction.h>

#include <ChimeraTK/BackendFactory.h>
#include <ChimeraTK/DMapFileParser.h>
#include <ChimeraTK/Exception.h>

#include "Exceptions.h"
#include <QDebug>
#include <QTextStream>
using namespace ChimeraTK;

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

QtHardMon::QtHardMon(bool noPrompts, QWidget* parent_, Qt::WindowFlags flags)
: QMainWindow(parent_, flags), ui(), dmapFileName_(), configFileName_(), insideReadOrWrite_(0),
  currentAccessorModel_(nullptr), _currentDeviceListItem(NULL), _plotWindow(NULL) {
  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();
  ui.setupUi(this);

  preferencesProvider.setValue("noPrompts", noPrompts);
  preferencesProvider.setValue("readOnClick", true);
  preferencesProvider.setValue("autoRead", true);
  preferencesProvider.setValue("maxWords", 0x10000);
  preferencesProvider.setValue("floatPrecision", static_cast<int>(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION));

  setWindowTitle("QtHardMon@" + QHostInfo::localHostName());
  setWindowIcon(QIcon(":/ChimeraTK_Logo_whitebg.png"));
  // we cannot scale the logo when creating a pixmap, so we use a fixed size png
  ui.logoLabel->setPixmap(QPixmap(":/ChimeraTK_Logo_whitebg.png"));

  addCopyActionForRegisterTreeWidget(); // Adds slot to copy qtreeiem's name to
                                        // clipboard

  connect(ui.deviceListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this,
      SLOT(deviceSelected(QListWidgetItem*, QListWidgetItem*)));

  connect(ui.registerTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this,
      SLOT(registerSelected(QTreeWidgetItem*, QTreeWidgetItem*)));

  connect(ui.registerTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this,
      SLOT(registerClicked(QTreeWidgetItem*)));

  connect(ui.propertiesWidget->ui.channelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(channelSelected(int)));

  connect(ui.loadBoardsButton, SIGNAL(clicked()), this, SLOT(loadBoards()));

  connect(ui.readButton, SIGNAL(clicked()), this, SLOT(read()));

  connect(ui.writeButton, SIGNAL(clicked()), this, SLOT(write()));

  connect(ui.aboutQtHardMonAction, SIGNAL(triggered()), this, SLOT(aboutQtHardMon()));

  connect(ui.aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));

  connect(ui.preferencesAction, SIGNAL(triggered()), this, SLOT(preferences()));

  connect(ui.loadConfigAction, SIGNAL(triggered()), this, SLOT(loadConfig()));

  connect(ui.saveConfigAction, SIGNAL(triggered()), this, SLOT(saveConfig()));

  connect(ui.saveConfigAsAction, SIGNAL(triggered()), this, SLOT(saveConfigAs()));

  connect(ui.loadBoardsAction, SIGNAL(triggered()), this, SLOT(loadBoards()));

  connect(ui.quitAction, SIGNAL(triggered()), this, SLOT(close()));

  connect(ui.openCloseButton, SIGNAL(clicked()), this, SLOT(openCloseDevice()));

  connect(ui.SortAscendingCheckBox, SIGNAL(stateChanged(int)), this, SLOT(handleSortCheckboxClick(int)));

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

  // customize table display
  //  customDelegate_.setDoubleSpinBoxPrecision(_floatPrecision);
  ui.propertiesWidget->ui.valuesTableView->setItemDelegate(&customDelegate_);

  _plotWindow = new PlotWindow(this);

  connect(ui.showPlotWindowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showPlotWindow(int)));

  connect(_plotWindow, SIGNAL(plotWindowClosed()), this, SLOT(unckeckShowPlotWindow()));

  ui.showDevicesWidget->hide();

  // also the plot window dfunctions are only enabled when a device is opened.
  _plotWindow->setEnabled(false);

  // Turn off automatic stretching of the device and content column.
  // It can only be done with the splitter handle.
  // Only the register tree is expanding automatically, like before
  ui.splitter->setStretchFactor(0,
      0);                              // the "showDevice" column, usually hidden
  ui.splitter->setStretchFactor(1, 0); // the device column itself
  ui.splitter->setStretchFactor(3, 0); // the device column itself
}

QtHardMon::~QtHardMon() {}

void QtHardMon::loadBoards() {
  // Show a file dialog to select the dmap file.
  QString dmapFileName = QFileDialog::getOpenFileName(
      this, tr("Open DeviceMap file"), ".", tr("DeviceMap files (*.dmap);; All files (*)"));

  if(dmapFileName.isEmpty()) {
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

bool QtHardMon::loadDmapFile(QString const& dmapFileName) {
  ChimeraTK::DMapFileParser fileParser;
  DeviceInfoMapPointer dmap;
  try {
    dmap = fileParser.parse(dmapFileName.toStdString());
    BackendFactory::getInstance().setDMapFilePath(dmapFileName.toStdString());
  }
  catch(ChimeraTK::logic_error& e) {
    showMessageBox(QMessageBox::Critical, QString("QtHardMon : Error"),
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

  for(DeviceInfoMap::iterator deviceIter = dmap->begin(); deviceIter != dmap->end(); ++deviceIter) {
    ui.deviceListWidget->addItem(
        new DeviceListItem((*deviceIter), (*deviceIter).deviceName.c_str(), ui.deviceListWidget));
  }
  return true;
  // on user request: do not automatically load the first device. This might be
  // not accessible and
  // immediately gives an error message.
}

void QtHardMon::deviceSelected(QListWidgetItem* deviceItem, QListWidgetItem* /*previousDeviceItem */) {
  ui.devicePropertiesGroupBox->setEnabled(true);

  // When the deviceListWidget is cleared , the currentItemChanged signal is
  // emitted with a null pointer. We have to catch this here and return. Before
  // returning we clear the device specific display info, close the device and
  // empty the register list.
  if(!deviceItem) {
    ui.deviceNameDisplay->setText("");
    ui.deviceIdentifierDisplay->setText("");
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

  DeviceListItem* deviceListItem = static_cast<DeviceListItem*>(deviceItem);

  _currentDeviceListItem = deviceListItem;
  // close the previous device. This also disables the relevant GUI elements
  closeDevice();
  // The register tree should be re-poulated. However if the device cannot be
  // opened, the old content would stay. So we clear it here.
  ui.registerTreeWidget->clear();
  // Also replace the device with a fresh one. Otherwise still the old register
  // tree will be picked up.
  currentDevice_ = ChimeraTK::Device();

  // Set the meta data before opening the device. If opening fails at least some
  // information as the device identifier is displayed.
  ui.deviceNameDisplay->setText(deviceListItem->getDeviceMapElement().deviceName.c_str());
  ui.deviceNameDisplay->setToolTip(deviceListItem->getDeviceMapElement().deviceName.c_str());
  ui.deviceIdentifierDisplay->setText(deviceListItem->getDeviceMapElement().uri.c_str());
  ui.deviceIdentifierDisplay->setToolTip(deviceListItem->getDeviceMapElement().uri.c_str());

  std::string absPath = deviceListItem->getDeviceMapElement().mapFileName;
  std::string mapFileName = extractFileNameFromPath(absPath);
  ui.mapFileDisplay->setText(mapFileName.c_str());
  ui.mapFileDisplay->setToolTip(absPath.c_str());

  // opening the device enables the gui elements if success
  openDevice(deviceListItem->getDeviceMapElement().deviceName);

  try {
    populateRegisterTree(deviceItem);
  }
  catch(Exception& e) {
    // In case anything fails, we would like to catch it and close the device.
    showMessageBox(QMessageBox::Critical, QString("QtHardMon Error"),
        QString("Could not load the list of registers for ") + deviceListItem->getDeviceMapElement().deviceName.c_str(),
        QString("Info: An exception was thrown: ") + e.what());
    closeDevice();
  }
  selectPreviousRegister();
}

void QtHardMon::selectPreviousRegister() {
  if(!(ui.autoselectPreviousRegisterCheckBox->isChecked())) {
    // don't re-select if this option is not enabled
    return;
  }
  // Searching a sub-tree does not work in QTreeWidget. So here is the
  // strategy: First get all register with the right name from the tree,
  // then pick the one where the full path matches. Ugly as hell, but easier
  // than writing an recursive search function ourself.
  auto registerPathComponents = _currentDeviceListItem->lastSelectedRegister.getComponents();
  if(registerPathComponents.empty()) {
    // nothing to be done, no previous register
    return;
  }

  auto registerName = registerPathComponents.back().c_str();
  // Get a list of all registers with this name.
  QList<QTreeWidgetItem*> registerList =
      ui.registerTreeWidget->findItems(registerName, Qt::MatchExactly | Qt::MatchRecursive);

  // Iterate the list until we find the right one
  for(auto reg : registerList) {
    // we know that there are only DeviceElementQTreeItems in the list, so we
    // can static cast
    auto deviceElement = static_cast<DeviceElementQTreeItem*>(reg);
    if(deviceElement->getRegisterPath() == _currentDeviceListItem->lastSelectedRegister) {
      ui.registerTreeWidget->setCurrentItem(reg);
      break;
    }
  }
}

void QtHardMon::openDevice(std::string const& deviceIdentifier) {
  // try to open a device. If this fails disable the buttons and
  // the registerValues
  try {
    currentDevice_.open(deviceIdentifier);
    // enable all of the GUI in case it was deactivated before
    ui.propertiesWidget->ui.valuesTableView->setEnabled(true);
    ui.operationsGroupBox->setEnabled(true);
    ui.optionsGroupBox->setEnabled(true);
    _plotWindow->setEnabled(true);

    ui.openClosedLabel->setText("Device is open.");
    ui.openCloseButton->setText("Close");
  }
  catch(Exception& e) {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString("Could not create the device ") + deviceIdentifier.c_str() + ".",
        QString("Info: An exception was thrown:") + e.what());
  }
}

void QtHardMon::closeDevice() {
  if(currentDevice_.isOpened()) currentDevice_.close();
  ui.propertiesWidget->ui.valuesTableView->setEnabled(false);
  ui.operationsGroupBox->setEnabled(false);
  ui.optionsGroupBox->setEnabled(false);
  _plotWindow->setEnabled(false);
  // we keep the register tree filled at the moment

  ui.openClosedLabel->setText("Device is closed.");
  ui.openCloseButton->setText("Open");
}

void QtHardMon::registerSelected(QTreeWidgetItem* registerItem, QTreeWidgetItem* /*previousRegisterItem */) {
  // Always clear the old data model. This is needed in all use cases below.
  ui.propertiesWidget->ui.valuesTableView->setModel(nullptr);
  delete currentAccessorModel_;
  currentAccessorModel_ = nullptr;

  // There is a case when a device entry is clicked in the device list, the slot
  // is called with a NULL registerItem
  if(!registerItem) {
    ui.propertiesWidget->clearFields();
    return;
  }

  // we know that the registerItem is a DeviceElementQTreeItem, so we can static
  // cast.
  DeviceElementQTreeItem* selectedItem = static_cast<DeviceElementQTreeItem*>(registerItem);
  ui.propertiesWidget->updateRegisterInfo(selectedItem->getRegisterInfo());
  if(selectedItem->getRegisterInfo()) {
    // there is valid register information. Create an accessor
    std::shared_ptr<RegisterTypeAbstractor> abstractAccessor;
    try {
      abstractAccessor = createAbstractAccessor(*(selectedItem->getRegisterInfo()), currentDevice_);
    }
    catch(Exception& e) {
      showMessageBox(QMessageBox::Critical, QString("QtHardMon : Error"),
          QString("Could not get register accessor for ") +
              static_cast<std::string>(selectedItem->getRegisterInfo()->getRegisterName()).c_str() + ".",
          QString("Info: An exception was thrown:") + e.what());
      return;
    }
    // If the data type is undefined or "noData" there is nothing to display for
    // QtHardMon. In this case we don't have an accessor (pointer is null) or a
    // data model.
    if(abstractAccessor) {
      // create a data model if we have an accessor.
      currentAccessorModel_ = new RegisterAccessorModel(this, abstractAccessor);
      ui.propertiesWidget->ui.valuesTableView->setModel(currentAccessorModel_);
    }
  }

  // set state of read/write buttons according to the register's capabilities
  if(selectedItem->getRegisterInfo()) {
    ui.readButton->setEnabled(selectedItem->getRegisterInfo()->isReadable());
    ui.writeButton->setEnabled(selectedItem->getRegisterInfo()->isWriteable());
  }
  else {
    ui.readButton->setEnabled(false);
    ui.writeButton->setEnabled(false);
  }

  // remember that this was the last selected register
  _currentDeviceListItem->lastSelectedRegister = selectedItem->getRegisterPath();

  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();

  // Only call autoread for registers (which have a valid accessor model).
  // Don't call it for modules (where  currentAccessorModel_ == nullptr)
  if(preferencesProvider.getValue<bool>("autoRead") && currentAccessorModel_) {
    read();
  }
}

void QtHardMon::read() {
  ++insideReadOrWrite_;
  // if no register is selected the accessor model is nullptr.
  if(!currentAccessorModel_) {
    showMessageBox(QMessageBox::Information, QString("QtHardMon Info"),
        QString("No register selected.                 "),
        QString("Please select a valid register. (Have you selected a module?)"));
    return;
  }
  try {
    currentAccessorModel_->read();
  }
  catch(std::exception& e) {
    closeDevice();
    ///@todo The error message accesses the _currentDeviceListItem. Is
    /// this safe? It might be NULL. On the other hand read() should only be
    /// active if a device is opened.
    showMessageBox(QMessageBox::Critical, QString("QtHardMon Error"),
        QString("Error reading from device ") + _currentDeviceListItem->getDeviceMapElement().uri.c_str() + ".",
        QString("Info: An exception was thrown:") + e.what() + QString("\n\nThe device has been closed."));
  }

  // check if plotting after reading is requested
  if(_plotWindow->isVisible() && _plotWindow->plotAfterReadIsChecked()) {
    _plotWindow->plot();
  }
  --insideReadOrWrite_;
}

void QtHardMon::write() {
  ++insideReadOrWrite_;
  // if no register is selected the accessor model is nullptr.
  if(!currentAccessorModel_) {
    showMessageBox(QMessageBox::Information, QString("QtHardMon Info"),
        QString("No register selected.                 "),
        QString("Please select a valid register. (Have you selected a module?)"));
    return;
  }

  try {
    currentAccessorModel_->write();
  }
  catch(std::exception& e) {
    closeDevice();

    ///@todo The error message accesses the _currentDeviceListItem. Is this
    /// safe? It
    /// might be NULL. On the other hand write() should only be active
    /// if a device is opened.
    showMessageBox(QMessageBox::Critical, QString("QtHardMon Error"),
        QString("Error writing to device ") + _currentDeviceListItem->getDeviceMapElement().uri.c_str() + ".",
        QString("Info: An exception was thrown:") + e.what() + QString("\n\nThe device has been closed."));
  }

  if(ui.readAfterWriteCheckBox->isChecked()) {
    read();
  }
  else {
    ui.propertiesWidget->clearDataWidgetBackground();
  }

  --insideReadOrWrite_;
}

void QtHardMon::preferences() {
  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();

  // create a preferences dialog and set the correct warning message with
  // contains the default number for maxWords.
  QDialog preferencesDialog;
  Ui::PreferencesDialogForm preferencesDialogForm;
  preferencesDialogForm.setupUi(&preferencesDialog);
  preferencesDialogForm.maxWordsWarningLabel->setText(QString("WARNING:") +
      " Setting this value too high can exhaust your memory, " +
      "which will lead to a segmentation fault. Default value is " + QString::number(0x10000));

  preferencesDialogForm.fontSizeSpinBox->setValue(font().pointSize());
  preferencesDialogForm.autoReadCheckBox->setChecked(preferencesProvider.getValue<bool>("autoRead"));
  preferencesDialogForm.readOnClickCheckBox->setChecked(preferencesProvider.getValue<bool>("readOnClick"));

  // set up the current value of maxWords
  preferencesDialogForm.maxWordsSpinBox->setMaximum(INT_MAX);
  preferencesDialogForm.maxWordsSpinBox->setValue(preferencesProvider.getValue<int>("maxWords"));

  // set up the floating point display decimal places
  preferencesDialogForm.precisionSpinBox->setMinimum(1);  // minimum one decimal place display
  preferencesDialogForm.precisionSpinBox->setMaximum(10); // maximum 10 decimal places
  preferencesDialogForm.precisionSpinBox->setValue(preferencesProvider.getValue<int>("floatPrecision"));

  int dialogResult = preferencesDialog.exec();

  // only set the values if ok has been pressed
  if(dialogResult == QDialog::Accepted) {
    preferencesProvider.setValue("maxWords", preferencesDialogForm.maxWordsSpinBox->value());

    // Read and set precision for delegate class
    preferencesProvider.setValue("floatPrecision", preferencesDialogForm.precisionSpinBox->value());

    customDelegate_.setDoubleSpinBoxPrecision(preferencesProvider.getValue<int>("floatPrecision"));

    // call registerSelected() so the size of the valuesList is adapted and
    // possible missing values are read
    // from the device
    registerSelected(ui.registerTreeWidget->currentItem(), ui.registerTreeWidget->currentItem());

    QFont newFont(this->font());
    newFont.setPointSize(preferencesDialogForm.fontSizeSpinBox->value());
    QApplication::setFont(newFont);

    preferencesProvider.setValue("readOnClick", preferencesDialogForm.readOnClickCheckBox->isChecked());
    preferencesProvider.setValue("autoRead", preferencesDialogForm.autoReadCheckBox->isChecked());
  }
}

void QtHardMon::aboutQtHardMon() {
  QMessageBox::about(this, "About QtHardMon", QString("QtHardMon Version ") + QT_HARD_MON_VERSION);
}

void QtHardMon::aboutQt() {
  QMessageBox::aboutQt(this, "About Qt");
}

void QtHardMon::loadConfig() {
  // the local, temporary variable, without underscore
  QString configFileName = QFileDialog::getOpenFileName(
      this, tr("Open QtHardMon config file"), ".", tr("HardMon config files (*.cfg);; All files (*)"));

  if(configFileName.isEmpty()) {
    // No file name selected, just quit.
    return;
  }

  loadConfig(configFileName);
}

void QtHardMon::loadConfig(QString const& configFileName) {
  ConfigFileReaderWriter configReader;
  try {
    // even the constructor can throw when opening and reading a file
    configReader.read(configFileName.toStdString());
  }
  catch(std::ifstream::failure& e) {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString("Could not read config file ") + configFileName + ".",
        QString("Info: An exception was thrown: ") + e.what());
    return;
  }

  // At this point we considder the config file as successfully loaded, even
  // though it might not contain any
  // valid parameters. But we store the file name.
  configFileName_ = configFileName;

  // show message box with parse errors, but just continue normally
  if(!configReader.getBadLines().isEmpty()) {
    QString infoText;
    for(QStringList::const_iterator badLinesIter = configReader.getBadLines().begin();
        badLinesIter != configReader.getBadLines().end();
        ++badLinesIter) {
      infoText += *badLinesIter;
      infoText += "\n";
    }

    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString("The following lines from the config file ") + configFileName +
            " are invalid and will be ignored. Please fix your config file.\n",
        infoText);
    return;
  }

  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();

  // first handle all settings that do not depend on opening a device map

  preferencesProvider.setValue("floatPrecision",
      configReader.getValue(PRECISION_INDICATOR_STRING, preferencesProvider.getValue<int>("floatPrecision")));

  // store in a local variable for now
  int maxWords = configReader.getValue(MAX_WORDS_STRING, preferencesProvider.getValue<int>("maxWords"));
  // check for validity. Minimum reasonable value is 1.
  if(maxWords >= 1) {
    // only after checking set the class wide maxWords variable
    preferencesProvider.setValue("maxWords", maxWords);
    // Update the register, so the length of the valuesList is adapted.
    // If another register is loaded from the config this might be repeated.
    // But for an easier logic we take this little overhead.
    registerSelected(ui.registerTreeWidget->currentItem(), ui.registerTreeWidget->currentItem());
  }
  else {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString("Read invalid maxWords from config file."), QString(""));
  }

  int readAfterWriteFlag =
      configReader.getValue(READ_AFTER_WRITE_STRING, ui.readAfterWriteCheckBox->isChecked() ? 1 : 0);
  // we use the implicit conversion 0=false, everyting else is true
  ui.readAfterWriteCheckBox->setChecked(readAfterWriteFlag);

  int showPlotWindowFlag =
      configReader.getValue(SHOW_PLOT_WINDOW_STRING, ui.showPlotWindowCheckBox->isChecked() ? 1 : 0);
  ui.showPlotWindowCheckBox->setChecked(showPlotWindowFlag);

  int plotAfterReadFlag = configReader.getValue(PLOT_AFTER_READ_STRING, _plotWindow->plotAfterReadIsChecked() ? 1 : 0);
  _plotWindow->setPlotAfterRead(plotAfterReadFlag);

  int autoselectPreviousRegisterFlag = configReader.getValue(
      AUTOSELECT_PREVIOUS_REGISTER_STRING, ui.autoselectPreviousRegisterCheckBox->isChecked() ? 1 : 0);
  ui.autoselectPreviousRegisterCheckBox->setChecked(autoselectPreviousRegisterFlag);

  preferencesProvider.setValue("readOnClick",
      static_cast<bool>(
          configReader.getValue(READ_ON_CLICK_STRING, preferencesProvider.getValue<bool>("readOnClick") ? 1 : 0)));
  preferencesProvider.setValue("autoRead",
      static_cast<bool>(
          configReader.getValue(AUTO_READ_STRING, preferencesProvider.getValue<bool>("autoRead") ? 1 : 0)));

  int fontSize = configReader.getValue(FONT_SIZE_STRING, font().pointSize());
  // Check validity of the font size.
  if(fontSize < 1 || fontSize > 99) {
  }
  else {
    QFont newFont(this->font());
    newFont.setPointSize(fontSize);
    QApplication::setFont(newFont);
  }

  // now read the mapping file, device and register. If anything goes wrong we
  // can just exit the function because
  // all other variables have already been processed.
  std::string dmapFileString = configReader.getValue(DMAP_FILE_STRING, std::string());

  if(dmapFileString.empty()) {
    // no dmap file. Just return, also loading device and register does not make
    // sense without dmap
    return;
  }

  if(!loadDmapFile(dmapFileString.c_str())) {
    // Just return, also loading device and register does not make sense without
    // dmap
    return;
  }

  // loop all devices and try to determine the last used register
  for(int deviceRow = 0; deviceRow < ui.deviceListWidget->count(); ++deviceRow) {
    DeviceListItem* deviceListItem = static_cast<DeviceListItem*>(ui.deviceListWidget->item(deviceRow));

    // determine the last register
    std::string deviceRegisterString = deviceListItem->getDeviceMapElement().deviceName + REGISTER_EXTENSION_STRING;
    deviceListItem->lastSelectedRegister = configReader.getValue(deviceRegisterString, std::string());
  } // for deviceRow

  // search for the device string
  std::string currentDeviceString = configReader.getValue(CURRENT_DEVICE_STRING, std::string());

  if(currentDeviceString.empty()) {
    // no device specified, noting left to do
    return;
  }

  // try to find the device in the list
  QList<QListWidgetItem*> matchingDevices =
      ui.deviceListWidget->findItems(currentDeviceString.c_str(), Qt::MatchExactly);

  if(matchingDevices.isEmpty()) {
    // the item should have been there, give a warning
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString("Device ") + currentDeviceString.c_str() + " is not in the dmap file.", QString(""));
    return;
  }

  // It's safe to call static_cast because the deviceListWidget is controlled by
  // this application.
  // We know that it is a deviceListItem, no nynamic checking needed.
  // We also know that matchingDevices.begin() is valid because the list is not
  // empty.
  // In case there is more than one entry with the same name we just pick the
  // first one.
  DeviceListItem* deviceListItem = static_cast<DeviceListItem*>(*matchingDevices.begin());

  // now we are ready to select the device
  ui.deviceListWidget->setCurrentItem(deviceListItem);
}

void QtHardMon::saveConfig() {
  if(configFileName_.isEmpty()) {
    saveConfigAs();
  }
  else {
    writeConfig(configFileName_);
  }
}

void QtHardMon::saveConfigAs() {
  // the local, temporary variable, without underscore
  QString configFileName = QFileDialog::getSaveFileName(
      this, tr("Save QtHardMon configuration"), ".", tr("HardMon config files (*.cfg);; All files (*)"));

  if(configFileName.isEmpty()) {
    // No file name selected, just quit.
    return;
  }

  // The file name seems valid. Now try to write it and store the name in the
  // class wide variable if successful.
  try {
    writeConfig(configFileName);
  }
  catch(std::ifstream::failure& e) {
    // a message box with the error warning has already been displayed. Nothing
    // more to do but quit.
    return;
  }

  // only store the file name upon successful writing (to the class wide
  // variable with the unterscore)
  configFileName_ = configFileName;
}

void QtHardMon::writeConfig(QString const& fileName) {
  ConfigFileReaderWriter configWriter;

  // set the variables we want do write to file

  // only write the file name when it's not empty, otherwise we get an invalid
  // line
  if(!dmapFileName_.isEmpty()) {
    configWriter.setValue(DMAP_FILE_STRING, dmapFileName_.toStdString());
  }

  // The device list widget only contains deviceListItems, so it's safe to use a
  // static cast here.
  DeviceListItem* deviceListItem = static_cast<DeviceListItem*>(ui.deviceListWidget->currentItem());

  // the device list might be emtpy, or there is no current item (?, is this
  // possibe?)

  if(deviceListItem) {
    configWriter.setValue(CURRENT_DEVICE_STRING, deviceListItem->getDeviceMapElement().deviceName);
    // writing a register without item does not make sense, so we keep it in the
    // if block
    //    configWriter.setValue(CURRENT_REGISTER_ROW_STRING,
    //    ui.registerTreeWidget->currentRow());
  }

  // add a value to store the last register for each device
  for(int deviceRow = 0; deviceRow < ui.deviceListWidget->count(); ++deviceRow) {
    deviceListItem = static_cast<DeviceListItem*>(ui.deviceListWidget->item(deviceRow));

    // Only write to the config file if the 'last selected' strings are not
    // empty.
    // Empty strings would cause a parse error, and if the entry is not found it
    // falls back to empty string anyway.
    if(!deviceListItem->lastSelectedRegister.getComponents().empty()) {
      std::string deviceRegisterString = deviceListItem->getDeviceMapElement().deviceName + REGISTER_EXTENSION_STRING;
      configWriter.setValue(deviceRegisterString, std::string(deviceListItem->lastSelectedRegister));
    }
  }

  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();

  configWriter.setValue(MAX_WORDS_STRING, preferencesProvider.getValue<int>("maxWords"));
  configWriter.setValue(PRECISION_INDICATOR_STRING, preferencesProvider.getValue<int>("floatPrecision"));
  configWriter.setValue(READ_AFTER_WRITE_STRING, ui.readAfterWriteCheckBox->isChecked() ? 1 : 0);
  configWriter.setValue(SHOW_PLOT_WINDOW_STRING, ui.showPlotWindowCheckBox->isChecked() ? 1 : 0);
  configWriter.setValue(
      AUTOSELECT_PREVIOUS_REGISTER_STRING, ui.autoselectPreviousRegisterCheckBox->isChecked() ? 1 : 0);
  configWriter.setValue(PLOT_AFTER_READ_STRING, _plotWindow->plotAfterReadIsChecked() ? 1 : 0);
  configWriter.setValue(FONT_SIZE_STRING, font().pointSize());
  configWriter.setValue(AUTO_READ_STRING, preferencesProvider.getValue<bool>("autoRead") ? 1 : 0);
  configWriter.setValue(READ_ON_CLICK_STRING, preferencesProvider.getValue<bool>("readOnClick") ? 1 : 0);

  // this
  try {
    configWriter.write(fileName.toStdString());
  }
  // we catch a write failure here to show a message box, but rethrow the
  // exception
  catch(std::ifstream::failure& e) {
    showMessageBox(QMessageBox::Critical, QString("QtHardMon : Error"),
        QString("Could not write config file ") + fileName + ".", QString("Info: An exception was thrown:") + e.what());

    // rethrow the exception so the calling code knows that writing failed.
    throw;
  }
}

void QtHardMon::showPlotWindow(int checkState) {
  if(checkState == Qt::Unchecked) {
    _plotWindow->setVisible(false);
  }
  else {
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
QtHardMon::DeviceListItem::DeviceListItem(ChimeraTK::DeviceInfoMap::DeviceInfo const& device_map_emlement,
    QListWidget* parent_)
: QListWidgetItem(parent_, DeviceListItemType), _deviceMapElement(device_map_emlement)

{}

QtHardMon::DeviceListItem::DeviceListItem(
    ChimeraTK::DeviceInfoMap::DeviceInfo const& device_map_emlement, const QString& text_, QListWidget* parent_)
: QListWidgetItem(text_, parent_, DeviceListItemType), _deviceMapElement(device_map_emlement) {}

QtHardMon::DeviceListItem::DeviceListItem(ChimeraTK::DeviceInfoMap::DeviceInfo const& device_map_emlement,
    const QIcon& icon_, const QString& text_, QListWidget* parent_)
: QListWidgetItem(icon_, text_, parent_, DeviceListItemType), _deviceMapElement(device_map_emlement) {}

QtHardMon::DeviceListItem::~DeviceListItem() {}

ChimeraTK::DeviceInfoMap::DeviceInfo const& QtHardMon::DeviceListItem::getDeviceMapElement() const {
  return _deviceMapElement;
}

void QtHardMon::registerClicked(QTreeWidgetItem* /*registerItem*/) {
  // Do not execute the read if the corresponding flag is off
  // registerSelected method.

  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();

  if(!preferencesProvider.getValue<bool>("readOnClick")) {
    return;
  }

  read();
}

void QtHardMon::channelSelected(int channelNumber) {
  if(currentAccessorModel_) {
    currentAccessorModel_->setChannelNumber(channelNumber);
  }
}

void QtHardMon::openCloseDevice() {
  if(currentDevice_.isOpened()) {
    closeDevice();
  }
  else {
    openDevice(_currentDeviceListItem->getDeviceMapElement().deviceName);
    // selectPreviousRegister() is not part of openDevice because the register
    // tree is not populated yet when openDevice is called inside
    // deviceSelected(). So we have to call it explicitly here.
    selectPreviousRegister();
  }
}

void QtHardMon::parseArgument(QString const& fileName) {
  if(checkExtension(fileName, ".dmap") == true) {
    (void)loadDmapFile(fileName);
  }
  else if(checkExtension(fileName, ".cfg") == true) {
    loadConfig(fileName);
  }
  else {
    showMessageBox(QMessageBox::Warning, QString("QtHardMon : Warning"),
        QString("Unsupported file extension provided. Filename will be ignored."), QString(""));
  }
}

bool QtHardMon::checkExtension(QString const& fileName, QString extension) {
  QStringRef extensionOfProvidedFile(&fileName, (fileName.size() - extension.size()), extension.size());
  bool areStringsEqual = (extension.compare(extensionOfProvidedFile) == 0);
  return areStringsEqual;
}

std::string QtHardMon::extractFileNameFromPath(const std::string& fileName) {
  std::string extractedName = fileName;
  size_t position = fileName.find_last_of('/');
  if(position != std::string::npos) {
    extractedName = fileName.substr(position + 1, std::string::npos);
  }
  return extractedName;
}

void QtHardMon::populateRegisterTree(QListWidgetItem* deviceItem) {
  if(deviceItem == NULL) {
    return;
  }

  // the deviceItem actually is a DeviceListItemType. As this is a private slot
  // it is safe to assume this and use a static cast.
  DeviceListItem* deviceListItem = static_cast<DeviceListItem*>(deviceItem);

  ui.registerTreeWidget->clear();

  const ChimeraTK::RegisterCatalogue registerCatalogue = currentDevice_.getRegisterCatalogue();
  // get the registerMap and fill the RegisterTreeWidget
  for(RegisterCatalogue::const_iterator registerIter = currentDevice_.getRegisterCatalogue().begin();
      registerIter != currentDevice_.getRegisterCatalogue().end();
      ++registerIter) {
    // parentNode can be null if there is no parent, i.e. the register is
    // directly in the treeWidget
    auto parentNode = RegisterTreeUtilities::getDeepestBranchNode(
        registerCatalogue.getRegister(registerIter->getRegisterName()), ui.registerTreeWidget);
    if(parentNode) {
      new DeviceElementQTreeItem(parentNode,
          registerIter->getRegisterName().getComponents().back().c_str(),
          registerCatalogue.getRegister(registerIter->getRegisterName()));
    }
    else {
      // no parent node. Directly add to the tree widget.
      new DeviceElementQTreeItem(ui.registerTreeWidget,
          registerIter->getRegisterName().getComponents().back().c_str(),
          registerCatalogue.getRegister(registerIter->getRegisterName()));
    }
  }
  ui.registerTreeWidget->expandAll();

  // Do NOT select a register. This is intentional!
  // It happens in
}

void QtHardMon::addCopyActionForRegisterTreeWidget() {
  QAction* copy = new QAction(tr("&Copy"), ui.registerTreeWidget);
  copy->setShortcuts(QKeySequence::Copy);
  copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(copy, SIGNAL(triggered()), this, SLOT(copyRegisterTreeItemNameToClipBoard()));
  ui.registerTreeWidget->addAction(copy);
}

void QtHardMon::copyRegisterTreeItemNameToClipBoard() {
  QTreeWidgetItem* currentItem = ui.registerTreeWidget->currentItem();
  if(currentItem) {
    QClipboard* clipboard = QApplication::clipboard();

    clipboard->clear(QClipboard::Clipboard);
    // clear the 'selection clipboard'; basically content that you get on
    // center mouse click
    clipboard->clear(QClipboard::Selection);

    ChimeraTK::RegisterPath registerPath = currentItem->text(0).toStdString();
    // loop the tree to add the parents
    QTreeWidgetItem* parent = currentItem->parent();
    while(parent) {
      registerPath = parent->text(0).toStdString() + registerPath;
      parent = parent->parent();
    }
    // copy to the standard clipboard and the mouse clipboard
    clipboard->setText(std::string(registerPath).c_str());
    clipboard->setText(std::string(registerPath).c_str(), QClipboard::Selection);
  }
  return;
}

void QtHardMon::handleSortCheckboxClick(int state) {
  if(state == Qt::Checked) {
    ui.registerTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    ui.registerTreeWidget->setSortingEnabled(true);
  }
  else if(state == Qt::Unchecked) {
    ui.registerTreeWidget->setSortingEnabled(false);
    // redraw the currently sorted tree to pick up the order from the mapfile.
    populateRegisterTree(ui.deviceListWidget->currentItem());
  }
}

void QtHardMon::showMessageBox(
    QMessageBox::Icon boxType, QString boxTitle, QString boxText, QString boxInformativeText) {
  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();

  if(preferencesProvider.getValue<bool>("noPrompts")) {
    std::cout << boxText.toStdString() << " " << boxInformativeText.toStdString() << std::endl;
  }
  else {
    QMessageBox messageBox(boxType, boxTitle, boxText, QMessageBox::Ok, this);
    messageBox.setInformativeText(boxInformativeText);
    messageBox.exec();
  }
}
