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

    QtHardmon_fixtureBase() {
        int argc = 0;
        char ** argv;
        app = new QApplication(argc, argv);
        qtHardMon = new QtHardMon;

        // Seems not necessary to run the tests
        // qtHardMon->show();
        // app->exec();
    }

};


struct QtHardMon_populatesDeviceList_fixture : public QtHardmon_fixtureBase {
    std::string dmapFile;
    
    QtHardMon_populatesDeviceList_fixture(const std::string & DmapFile) :
    dmapFile(DmapFile)
    {
        // First, we want to load DMap file. Normally, we would click "Load Boards" button, 
        // which would prompt us for a .dmap file and then pass the value to loadDmapFile 
        // method, OR we would pass it as an argument when starting QtHardMon.
        // For the purpose of that test, we are directly calling loadDmapFile method.
    //    qtHardMon->loadDmapFile(QString(dmapFile.c_str()));
    }
};

/*
 * The devices of various backends are read properly and populate the device list.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesDeviceList )
{
    QtHardMon_populatesDeviceList_fixture fixtureNumerical("test_files/test_QtHardMon_valid_dummy_lmap.dmap");

    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon->_hardMonForm.deviceListWidget->count(), 2);
}

struct QtHardmon_populatesRegisterTree_fixture : public QtHardmon_fixtureBase {
    std::string dmapFile;
    std::string deviceNameToSelect;
    
    QtHardmon_populatesRegisterTree_fixture(const std::string & DmapFile, const std::string & DeviceNameToSelect) :
    dmapFile(DmapFile),
    deviceNameToSelect(DeviceNameToSelect)
    {
        // First, we want to load DMap file. Normally, we would click "Load Boards" button, 
        // which would prompt us for a .dmap file and then pass the value to loadDmapFile 
        // method, OR we would pass it as an argument when starting QtHardMon.
        // For the purpose of that test, we are directly calling loadDmapFile method.
        qtHardMon->loadDmapFile(QString(dmapFile.c_str()));
    }
};

/*
 * When device is selected, the register tree gets populated properly.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesRegisterTree )
{

    QtHardmon_populatesRegisterTree_fixture fixtureNumerical("test_files/test_QtHardMon_populatesRegisterTree.dmap", "NUMDEV");

    // Expecting BOARD, APP0, MODULE0 and MODULE1 modules
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItemCount(), 4);
    // BOARD has 2 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 2);
    // APP0 has 4 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(1)->childCount(), 4);
    // MODULE0 has 2 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
    // MODULE1 has 3 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(3)->childCount(), 3);

    QtHardmon_populatesRegisterTree_fixture fixtureNonNumerical("test_files/test_QtHardMon_populatesRegisterTree.dmap", "LMAPDEV");

    // Expecting MyModule and module, that contains all uncategorized registers
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItemCount(), 2);
    // Uncategorized has 5 items: 3 registers and 2 constants
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 5);
    // MyModule has two items: 1 register and 1 submodule
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon->_hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
}
