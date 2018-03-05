#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_QtHardMon

#include <boost/test/unit_test.hpp>

#include <QtGui>
#include <tuple> // std::tuple

#include "TestUtilities.h"

#include "PreferencesProvider.h"
#include "QtHardMon.h"

/*
 * This base class is used for all of the fixtures in Qt testing due to the
 * necessity of initializing QApplication instance.
 */
struct QtHardmon_fixtureBase {
  QApplication *app;
  QtHardMon *qtHardMon;

  QtHardmon_fixtureBase(bool noPrompts = true) {
    int argc = 0;
    char **argv = nullptr;
    app = new QApplication(argc, argv);
    if (!noPrompts) {
      qtHardMon = new QtHardMon;
    } else {
      qtHardMon = new QtHardMon(true);
    }

    // Seems not necessary to run the tests
    // qtHardMon->show();
    // app->exec();
  }
};

/*
 * The window is properly filled with no data and proper states of particular
 * window components.
*/
BOOST_AUTO_TEST_CASE(QtHardMon_emptyUponConstruction) {
  QtHardmon_fixtureBase fixture(false);

  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.deviceListWidget->count(), 0);
  BOOST_CHECK_EQUAL(
      fixture.qtHardMon->ui.registerTreeWidget->topLevelItemCount(), 0);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.deviceStatusGroupBox->isEnabled(),
                    false);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.openCloseButton->isEnabled(), false);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.devicePropertiesGroupBox->isEnabled(),
                    false);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.loadBoardsButton->isEnabled(), true);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.SortAscendingcheckBox->isChecked(),
                    true);
  BOOST_CHECK_EQUAL(
      fixture.qtHardMon->ui.autoselectPreviousRegisterCheckBox->isChecked(),
      false);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.registerPropertiesWidget->isEnabled(),
                    true);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.registerPropertiesWidget->ui
                        ->valuesTableWidget->rowCount(),
                    0);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.optionsGroupBox->isEnabled(), false);
  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.operationsGroupBox->isEnabled(),
                    false);

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();
  BOOST_CHECK_EQUAL(preferencesProvider.getValue<bool>("noPrompts"), false);
}

/*
 * The initial state of settings is set to particular values.
*/
BOOST_AUTO_TEST_CASE(QtHardMon_defaultSettings) {
  QtHardmon_fixtureBase fixture;

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  BOOST_CHECK_EQUAL(preferencesProvider.getValue<int>("maxWords"), 0x10000);
  BOOST_CHECK_EQUAL(preferencesProvider.getValue<int>("floatPrecision"),
                    CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION);
  BOOST_CHECK_EQUAL(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION, 4);

  // Not passing via ssh connection due to different font size.
  // BOOST_CHECK_EQUAL(fixture.qtHardMon->font().pointSize(), 11);

  BOOST_CHECK_EQUAL(preferencesProvider.getValue<bool>("autoRead"), true);
  BOOST_CHECK_EQUAL(preferencesProvider.getValue<bool>("readOnClick"), true);

  BOOST_CHECK_EQUAL(preferencesProvider.getValue<bool>("noPrompts"), true);
}

struct QtHardMon_populatesDeviceList_fixture : public QtHardmon_fixtureBase {
  std::string dmapFile;

  QtHardMon_populatesDeviceList_fixture(const std::string &DmapFile)
      : dmapFile(DmapFile) {
    // First, we want to load DMap file. Normally, we would click "Load Boards"
    // button,
    // which would prompt us for a .dmap file and then pass the value to
    // loadDmapFile
    // method, OR we would pass it as an argument when starting QtHardMon.
    // For the purpose of that test, we are directly calling loadDmapFile
    // method.
    QDir::setCurrent(
        QFileInfo(("test_files/" + dmapFile).c_str()).absolutePath());
    qtHardMon->loadDmapFile(QString(dmapFile.c_str()));
  }
};

/*
 * The devices of various backends are read properly and populate the device
 * list.
*/
BOOST_AUTO_TEST_CASE(QtHardMon_populatesDeviceList) {
  QtHardMon_populatesDeviceList_fixture fixture(
      "test_files/test_QtHardMon_valid_dummy.dmap");

  BOOST_CHECK_EQUAL(fixture.qtHardMon->ui.deviceListWidget->count(), 3);
}

BOOST_AUTO_TEST_CASE(QtHardMon_properlyCatchesInvalidDeviceSelected) {
  QtHardMon_populatesDeviceList_fixture fixture(
      "test_files/test_QtHardMon_valid_dummy.dmap");

  QList<QListWidgetItem *> items =
      fixture.qtHardMon->ui.deviceListWidget->findItems("INVALID",
                                                        Qt::MatchExactly);
  if (items.size() > 0) {
    BOOST_CHECK_NO_THROW(
        fixture.qtHardMon->ui.deviceListWidget->setCurrentItem(items.at(0)));
  } else {
    BOOST_FAIL("Device \"INVALID\" not found, check test case implementation.");
  }
}
struct QtHardmon_populatesRegisterTree_fixture
    : public QtHardMon_populatesDeviceList_fixture {

  QtHardmon_populatesRegisterTree_fixture(const std::string &DmapFile,
                                          const std::string &DeviceNameToSelect,
                                          bool Sorted = true)
      : QtHardMon_populatesDeviceList_fixture(DmapFile) {
    qtHardMon->ui.SortAscendingcheckBox->setCheckState(Sorted ? Qt::Checked
                                                              : Qt::Unchecked);
    switchDeviceSelection(DeviceNameToSelect);
  }

  void switchDeviceSelection(const std::string &DeviceNameToSelect) {
    QList<QListWidgetItem *> items = qtHardMon->ui.deviceListWidget->findItems(
        DeviceNameToSelect.c_str(), Qt::MatchExactly);
    if (items.size() > 0) {
      qtHardMon->ui.deviceListWidget->setCurrentItem(items.at(0));
    } else {
      BOOST_FAIL("Device not found...");
    }
  }
};

/*
 * When device is selected, the register tree gets populated properly, sorted.
*/
BOOST_AUTO_TEST_CASE(QtHardMon_populatesRegisterTreeSorted) {
  QtHardmon_populatesRegisterTree_fixture fixtureSorted(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV");

  // Expecting BOARD, APP0, MODULE0 and MODULE1 modules
  BOOST_REQUIRE_EQUAL(
      fixtureSorted.qtHardMon->ui.registerTreeWidget->topLevelItemCount(), 4);
  // Modules are initially sorted, so APP0, BOARD, MODULE0, MODULE1

  // APP0 has 4 items
  BOOST_CHECK_EQUAL(
      fixtureSorted.qtHardMon->ui.registerTreeWidget->topLevelItem(0)
          ->childCount(),
      4);
  // BOARD has 2 items
  BOOST_CHECK_EQUAL(
      fixtureSorted.qtHardMon->ui.registerTreeWidget->topLevelItem(1)
          ->childCount(),
      2);
  // MODULE0 has 2 items
  BOOST_CHECK_EQUAL(
      fixtureSorted.qtHardMon->ui.registerTreeWidget->topLevelItem(2)
          ->childCount(),
      2);
  // MODULE1 has 3 items
  BOOST_CHECK_EQUAL(
      fixtureSorted.qtHardMon->ui.registerTreeWidget->topLevelItem(3)
          ->childCount(),
      3);
}

/*
 * When device is selected, the register tree gets populated properly. When
 * autosorting checkbox is unchecked, they are not sorted.
*/
BOOST_AUTO_TEST_CASE(QtHardMon_populatesRegisterTreeUnsorted) {
  QtHardmon_populatesRegisterTree_fixture fixtureUnsorted(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV", false);

  // Expecting BOARD, APP0, MODULE0 and MODULE1 modules
  BOOST_REQUIRE_EQUAL(
      fixtureUnsorted.qtHardMon->ui.registerTreeWidget->topLevelItemCount(), 4);
  // Modules are not sorted, so BOARD, APP0, MODULE0, MODULE1
  // BOARD has 2 items
  BOOST_CHECK_EQUAL(
      fixtureUnsorted.qtHardMon->ui.registerTreeWidget->topLevelItem(0)
          ->childCount(),
      2);
  // APP0 has 4 items
  BOOST_CHECK_EQUAL(
      fixtureUnsorted.qtHardMon->ui.registerTreeWidget->topLevelItem(1)
          ->childCount(),
      4);
  // MODULE0 has 2 items
  BOOST_CHECK_EQUAL(
      fixtureUnsorted.qtHardMon->ui.registerTreeWidget->topLevelItem(2)
          ->childCount(),
      2);
  // MODULE1 has 3 items
  BOOST_CHECK_EQUAL(
      fixtureUnsorted.qtHardMon->ui.registerTreeWidget->topLevelItem(3)
          ->childCount(),
      3);
}

struct QtHardmon_populatesRegisterProperties_fixture
    : public QtHardmon_populatesRegisterTree_fixture {
  QTreeWidgetItem *registerToBeFound;

  QtHardmon_populatesRegisterProperties_fixture(
      const std::string &DmapFile, const std::string &DeviceNameToSelect,
      std::vector<std::string> RegisterToSelect)
      : QtHardmon_populatesRegisterTree_fixture(DmapFile, DeviceNameToSelect) {
    switchRegisterSelection(RegisterToSelect);
  }

  void switchRegisterSelection(std::vector<std::string> RegisterToSelect) {
    registerToBeFound = NULL;
    QList<QTreeWidgetItem *> registerList =
        qtHardMon->ui.registerTreeWidget->findItems(
            RegisterToSelect.back().c_str(),
            Qt::MatchExactly | Qt::MatchRecursive);
    RegisterToSelect.pop_back();

    for (std::vector<std::string>::reverse_iterator nameIter =
             RegisterToSelect.rbegin();
         nameIter != RegisterToSelect.rend(); ++nameIter) {
      // Iterate the list until we find the one with the right module

      for (QList<QTreeWidgetItem *>::iterator registerIter =
               registerList.begin();
           registerIter != registerList.end(); ++registerIter) {
        // if we found the right register select it and quit the loop
        if ((*registerIter)->parent()->text(0) == (*nameIter).c_str()) {
          registerToBeFound = *registerIter;
          break;
        }
      }
    }
    if (!registerToBeFound) {
      BOOST_FAIL("Register not found...");
    } else {
      qtHardMon->ui.registerTreeWidget->setCurrentItem(registerToBeFound);
    }
  }
};

/*
 * When selecting register, register properties are properly populated.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_populatesRegisterProperties) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV", {"APP0", "MODULE1"});

  TestUtilities::checkRegisterProperties(
      fixture.qtHardMon->ui.registerPropertiesWidget, "MODULE1", "APP0", "1",
      "32", "2", "8", "32", "0", "1");
}

/*
 * When toggling autoselect checkbox, previously selected register is properly
 * selected.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_autoselectsRegister) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV", {"MODULE1", "WORD_USER1"});

  TestUtilities::checkRegisterProperties(
      fixture.qtHardMon->ui.registerPropertiesWidget, "WORD_USER1", "MODULE1",
      "1", "32", "1", "4", "16", "3", "1");

  fixture.qtHardMon->ui.autoselectPreviousRegisterCheckBox->setCheckState(
      Qt::Checked);
  fixture.switchDeviceSelection("NUMDEV_MULT");

  TestUtilities::checkRegisterProperties(
      fixture.qtHardMon->ui.registerPropertiesWidget, "", "", "", "", "", "",
      "", "", "");

  fixture.switchDeviceSelection("NUMDEV");

  TestUtilities::checkRegisterProperties(
      fixture.qtHardMon->ui.registerPropertiesWidget, "WORD_USER1", "MODULE1",
      "1", "32", "1", "4", "16", "3", "1");
}

/*
 * When selecting register, data table is populated with valid values.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_populatesDataTable) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV", {"APP0", "MODULE1"});

  TestUtilities::checkTableData(
      fixture.qtHardMon->ui.registerPropertiesWidget,
      {std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0)});
}

/*
 * When changing data, rows are properly filled, including conversions.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_acceptsInsertedData) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV", {"APP0", "MODULE1"});

  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 2, std::make_tuple(10, 10, 10.0));
  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 0, std::make_tuple(3, 3, 3.0));

  fixture.switchRegisterSelection({"MODULE0", "WORD_USER1"});

  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 2, std::make_tuple(4, 4, 0.5));
  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 0, std::make_tuple(12, 12, 1.5));

  fixture.switchRegisterSelection({"MODULE0", "WORD_USER2"});

  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 2, std::make_tuple(4, 4, 0.125));
  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 0, std::make_tuple(12, 12, 0.375));
}

/*
 * When clicking Read, the register value is updated.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_readsRegister) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV", {"APP0", "MODULE1"});

  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 2, std::make_tuple(10, 10, 10.0));

  fixture.qtHardMon->read();

  TestUtilities::checkTableData(
      fixture.qtHardMon->ui.registerPropertiesWidget,
      {std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0)});
}

/*
 * When clicking Write, the register value is set in device.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_writesRegister) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV", {"APP0", "MODULE1"});

  TestUtilities::setTableValue(fixture.qtHardMon->ui.registerPropertiesWidget,
                               0, 2, std::make_tuple(10, 10, 10.0));

  fixture.qtHardMon->write();

  fixture.switchRegisterSelection({"MODULE0", "WORD_USER1"});
  fixture.switchRegisterSelection({"APP0", "MODULE1"});

  TestUtilities::checkTableData(
      fixture.qtHardMon->ui.registerPropertiesWidget,
      {std::make_tuple(10, 10, 10.0), std::make_tuple(0, 0, 0.0)});
}

/*
 * When device with multiplexed registers is selected, the register tree gets
 * populated properly.
*/
BOOST_AUTO_TEST_CASE(QtHardMon_populatesRegisterTreeMultiplexed) {
  QtHardmon_populatesRegisterTree_fixture fixtureMultiplexed(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT");

  // There are two modules, APP0 and MODULE1 (we care only for the first one
  // in
  // this test)
  BOOST_REQUIRE_EQUAL(
      fixtureMultiplexed.qtHardMon->ui.registerTreeWidget->topLevelItemCount(),
      3);

  // APP0 has 4 items, one of them is multiplexed area
  BOOST_REQUIRE_EQUAL(
      fixtureMultiplexed.qtHardMon->ui.registerTreeWidget->topLevelItem(0)
          ->childCount(),
      4);

  // Multiplexed area has 16 sequence registers
  BOOST_CHECK_EQUAL(
      fixtureMultiplexed.qtHardMon->ui.registerTreeWidget->topLevelItem(0)
          ->child(1)
          ->childCount(),
      16);
}

/*
 * When selecting sequence register, register properties are properly
 * populated.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_populatesSequenceRegisterProperties) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT",
      {"APP0", "AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA", "SEQUENCE_DAQ0_ADCA_10"});

  TestUtilities::checkRegisterProperties(
      fixture.qtHardMon->ui.registerPropertiesWidget, "SEQUENCE_DAQ0_ADCA_10",
      "APP0", "13", "1028", "4096", "4", "32", "0", "1");
}

/*
 * When selecting sequence register, data table is populated with valid
 * values.
 */
BOOST_AUTO_TEST_CASE(QtHardMon_populatesDataTableSequence) {
  QtHardmon_populatesRegisterProperties_fixture fixture(
      "test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT",
      {"APP0", "AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA", "SEQUENCE_DAQ0_ADCA_10"});

  TestUtilities::checkTableData(fixture.qtHardMon->ui.registerPropertiesWidget,
                                {std::make_tuple(0, 0, 0.0)}, 4096);
}

BOOST_AUTO_TEST_CASE(QtHardMon_populatesRegisterTreeNonNumerical) {
  QtHardmon_populatesRegisterTree_fixture fixtureNonNumerical(
      "test_QtHardMon_valid_dummy_lmap.dmap", "LMAPDEV");

  // Expecting MyModule and module, that contains all uncategorized registers
  BOOST_REQUIRE_EQUAL(
      fixtureNonNumerical.qtHardMon->ui.registerTreeWidget->topLevelItemCount(),
      2);
  // Uncategorized has 5 items: 2 registers and 2 constants
  BOOST_CHECK_EQUAL(
      fixtureNonNumerical.qtHardMon->ui.registerTreeWidget->topLevelItem(0)
          ->childCount(),
      4);
  // MyModule has 1 register and one submodule
  BOOST_REQUIRE_EQUAL(
      fixtureNonNumerical.qtHardMon->ui.registerTreeWidget->topLevelItem(1)
          ->childCount(),
      2);
  // Submodule has one register
  BOOST_CHECK_EQUAL(
      fixtureNonNumerical.qtHardMon->ui.registerTreeWidget->topLevelItem(1)
          ->child(0)
          ->childCount(),
      1);
}

/* ================================ SKIPPED =================================
 */
/*
 * Those tests are skipped, as they require changing values on the device
 * side.
 * Not very troublesome, but enough to leave them to be implemented someday
 * soon.
 */

/*
 * When Read After Write checkbox is checked, the register value is updated
 * after writing.
 */
// BOOST_AUTO_TEST_CASE ( QtHardMon_readsRegisterAfterWrite )
// {
//     // reads after write
//     // reads after write from file
// }

/*
 * When Read Continuously checkbox is checked, the register value is updated
 * in
 * intervals.
 */
// BOOST_AUTO_TEST_CASE ( QtHardMon_readsRegisterContinuously )
// {
// }

/*
 * Those tests require invoking windows, and for now it may be a problem to
 * invoke them without actually showing them.
 */

/*
 * The state of settings after changing them in the settings dialog is set to
 * particular values.
*/
// BOOST_AUTO_TEST_CASE ( QtHardMon_changeSettings )
// {
//     QtHardmon_fixtureBase fixture;

//     // change:
//     // _maxWords value
//     // _floatPrecision value
//     // _customDelegate float precision set
//     // font().pointSize()
//     // autoRead_ bool
//     // readOnClick_ bool
//     // check if they were saved
// }

/*
 * When Show Plot checkbox is checked, the plot is properly shown and filled.
 */
// BOOST_AUTO_TEST_CASE ( QtHardMon_showsRegisterPlot )
// {
// }

/*
 * Those tests are made to test new functionality, so they are skipped till
 * the
 * functionality is implemented.
 */

/*
 * Those tests are skipped since it looks like the buttons invoking the
 * functionality to be tested are greyed out.
 */

/*
 * When clicking Read From File, the register value is updated.
 */
// BOOST_AUTO_TEST_CASE ( QtHardMon_readsRegisterFromFile )
// {
// }

/*
 * When clicking Write From File, the register value is set in device.
 */
// BOOST_AUTO_TEST_CASE ( QtHardMon_writesRegisterFromFile )
// {
// }