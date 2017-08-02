#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_QtHardMon

#include <boost/test/unit_test.hpp>

#include <QtGui>

// Evil but awesome
#define private public

#include "QtHardMon.h"

/*
 * This base class is used for all of the fixtures in Qt testing due to the necessity of initializing QApplication instance.
 */
struct QtHardmon_fixtureBase {
    QApplication * app;
    QtHardMon * qtHardMon;

    QtHardmon_fixtureBase(bool noPrompts = true) {
        int argc = 0;
        char ** argv;
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
 * The window is properly filled with no data and proper states of particular window components.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_emptyUponConstruction )
{
    QtHardmon_fixtureBase fixture(false);

    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.deviceListWidget->count(), 0);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItemCount(), 0);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.deviceStatusGroupBox->isEnabled(), false);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.openCloseButton->isEnabled(), false);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.devicePropertiesGroupBox->isEnabled(), false);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.loadBoardsButton->isEnabled(), true);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.SortAscendingcheckBox->isChecked(), true);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.autoselectPreviousRegisterCheckBox->isChecked(), false);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.registerPropertiesGroupBox->isEnabled(), true);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.valuesTableWidget->rowCount(), 0);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.optionsGroupBox->isEnabled(), false);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.operationsGroupBox->isEnabled(), false);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->noPrompts_, false);
    
}


/*
 * The initial state of settings is set to particular values.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_defaultSettings )
{
    QtHardmon_fixtureBase fixture;

    BOOST_CHECK_EQUAL(fixture.qtHardMon->_maxWords, 0x10000);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_floatPrecision, CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION);
    BOOST_CHECK_EQUAL(CustomDelegates::DOUBLE_SPINBOX_DEFAULT_PRECISION, 4);
    
    // Not passing via ssh connection due to different font size.
    BOOST_CHECK_EQUAL(fixture.qtHardMon->font().pointSize(), 11);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_autoRead, true);
    BOOST_CHECK_EQUAL(fixture.qtHardMon->_readOnClick, true);
}


// SKIPPED - need to change the code to be test-ready
/*
 * The state of settings after changing them in the settings dialog is set to particular values.
*/
// BOOST_AUTO_TEST_CASE ( QtHardMon_changeSettings )
// {
//     QtHardmon_fixtureBase fixture;

//     // change:
//     // _maxWords value
//     // _floatPrecision value
//     // _customDelegate float precision set
//     // font().pointSize()
//     // _autoRead bool
//     // _readOnClick bool
//     // check if they were saved 
// }


struct QtHardMon_populatesDeviceList_fixture : public QtHardmon_fixtureBase {
    std::string dmapFile;
    
    QtHardMon_populatesDeviceList_fixture(const std::string & DmapFile) :
    dmapFile(DmapFile)
    {
        // First, we want to load DMap file. Normally, we would click "Load Boards" button, 
        // which would prompt us for a .dmap file and then pass the value to loadDmapFile 
        // method, OR we would pass it as an argument when starting QtHardMon.
        // For the purpose of that test, we are directly calling loadDmapFile method.
        QDir::setCurrent( QFileInfo(("test_files/" + dmapFile).c_str()).absolutePath() );
        qtHardMon->loadDmapFile(QString(dmapFile.c_str()));
    }
};

/*
 * The devices of various backends are read properly and populate the device list.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesDeviceList )
{
    QtHardMon_populatesDeviceList_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap");

    BOOST_CHECK_EQUAL(fixture.qtHardMon->_hardMonForm.deviceListWidget->count(), 2);
}

struct QtHardmon_populatesRegisterTree_fixture : public QtHardMon_populatesDeviceList_fixture {
    std::string deviceNameToSelect;
    
    QtHardmon_populatesRegisterTree_fixture(const std::string & DmapFile, const std::string & DeviceNameToSelect, bool Sorted = true) :
    QtHardMon_populatesDeviceList_fixture(DmapFile),
    deviceNameToSelect(DeviceNameToSelect)
    {
        qtHardMon->_hardMonForm.SortAscendingcheckBox->setCheckState(Sorted ? Qt::Checked : Qt::Unchecked);
        QList<QListWidgetItem *> items =  qtHardMon->_hardMonForm.deviceListWidget->findItems(DeviceNameToSelect.c_str(), Qt::MatchExactly);
        if (items.size() > 0) {
            std::cout << "Found!" << std::endl;
            qtHardMon->_hardMonForm.deviceListWidget->setCurrentItem(items.at(0));
        }
    }
};

/*
 * When device is selected, the register tree gets populated properly, sorted.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesRegisterTreeSorted )
{
    QtHardmon_populatesRegisterTree_fixture fixtureSorted("test_QtHardMon_valid_dummy.dmap", "NUMDEV");

    // Expecting BOARD, APP0, MODULE0 and MODULE1 modules
    BOOST_CHECK_EQUAL(fixtureSorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItemCount(), 4);
    // Modules are initially sorted, so APP0, BOARD, MODULE0, MODULE1

    // APP0 has 4 items
    BOOST_CHECK_EQUAL(fixtureSorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 4);
    // BOARD has 2 items
    BOOST_CHECK_EQUAL(fixtureSorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(1)->childCount(), 2);
    // MODULE0 has 2 items
    BOOST_CHECK_EQUAL(fixtureSorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
    // MODULE1 has 3 items
    BOOST_CHECK_EQUAL(fixtureSorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(3)->childCount(), 3);

    // QtHardmon_populatesRegisterTree_fixture fixtureNonNumerical("test_QtHardMon_valid_dummy_lmap.dmap", "LMAPDEV");

    // Expecting MyModule and module, that contains all uncategorized registers
    // BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItemCount(), 2);
    // Uncategorized has 5 items: 3 registers and 2 constants
   //  BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 5);
    // MyModule has two items: 1 register and 1 submodule
    // BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
}

/*
 * When device is selected, the register tree gets populated properly. When autosorting checkbox is unchecked, they are not sorted.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesRegisterTreeUnsorted )
{
    QtHardmon_populatesRegisterTree_fixture fixtureUnsorted("test_QtHardMon_valid_dummy.dmap", "NUMDEV", false);

    // Expecting BOARD, APP0, MODULE0 and MODULE1 modules
    BOOST_CHECK_EQUAL(fixtureUnsorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItemCount(), 4);
    // Modules are not sorted, so BOARD, APP0, MODULE0, MODULE1
    // BOARD has 2 items
    BOOST_CHECK_EQUAL(fixtureUnsorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 2);
    // APP0 has 4 items
    BOOST_CHECK_EQUAL(fixtureUnsorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(1)->childCount(), 4);
    // MODULE0 has 2 items
    BOOST_CHECK_EQUAL(fixtureUnsorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
    // MODULE1 has 3 items
    BOOST_CHECK_EQUAL(fixtureUnsorted.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(3)->childCount(), 3);
}

/*
 * When device with multiplexed registers is selected, the register tree gets populated properly.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesRegisterTreeMultiplexed )
{
    QtHardmon_populatesRegisterTree_fixture fixtureMultiplexed("test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT");

    // There is only APP0 module 
    BOOST_CHECK_EQUAL(fixtureMultiplexed.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItemCount(), 1);

    // APP0 has 4 items, one of them is multiplexed area
    BOOST_CHECK_EQUAL(fixtureMultiplexed.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 4);

    // Multiplexed area has 16 sequence registers
    BOOST_CHECK_EQUAL(fixtureMultiplexed.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(0)->child(1)->childCount(), 16);
}

/*
 * When toggling autoselect checkbox, previously selected register is properly selected.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_autoselectsRegister )
{
    
}

/*
 * When selecting register, register properties are properly populated.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesRegisterProperties )
{
}

/*
 * When selecting register, data tables is populated with valid values.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesDataTable )
{
}


/*
 * When clicking Read, the register value is updated.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_readsRegister )
{
}

/*
 * When clicking Write, the register value is set in device.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_writesRegister )
{
}


/*
 * When clicking Read From File, the register value is updated.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_readsRegisterFromFile )
{
}

/*
 * When clicking Write From File, the register value is set in device.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_writesRegisterFromFile )
{
}

/*
 * When Read After Write checkbox is checked, the register value is updated after writing.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_readsRegisterAfterWrite )
{
    // reads after write
    // reads after write from file
}

/*
 * When Read Continuously checkbox is checked, the register value is updated in intervals.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_readsRegisterContinuously )
{
}

/*
 * When Show Plot checkbox is checked, the plot is properly shown and filled.
 */ 
BOOST_AUTO_TEST_CASE ( QtHardMon_showsRegisterPlot )
{
}