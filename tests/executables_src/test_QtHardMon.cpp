#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_QtHardMon

#include <boost/test/unit_test.hpp>

#define private public
#include "QtHardMon.h"


struct QtHardmon_populatesRegisterTree_fixture {

    QtHardMon qtHardMon;
    std::string dmapFile;
    std::string deviceNameToSelect;
    
    QtHardmon_populatesRegisterTree_fixture(const std::string & DmapFile, const std::string & DeviceNameToSelect) :
    dmapFile(DmapFile),
    deviceNameToSelect(DeviceNameToSelect)
    {

    }

};

/*
 * When device is selected, the register tree gets populated properly.
*/
BOOST_AUTO_TEST_CASE ( QtHardMon_populatesRegisterTree )
{

    QtHardmon_populatesRegisterTree_fixture fixtureNumerical("test_QtHardMon_populatesRegisterTree.dmap", "NUMDEV");

    // Expecting BOARD, APP0, MODULE0 and MODULE1 modules
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItemCount(), 4);
    // BOARD has 2 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 2);
    // APP0 has 4 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(1)->childCount(), 4);
    // MODULE0 has 2 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
    // MODULE1 has 3 items
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(3)->childCount(), 3);

    QtHardmon_populatesRegisterTree_fixture fixtureNonNumerical("test_QtHardMon_populatesRegisterTree.dmap", "LMAPDEV");

    // Expecting MyModule and module, that contains all uncategorized registers
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItemCount(), 2);
    // Uncategorized has 5 items: 3 registers and 2 constants
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 5);
    // MyModule has two items: 1 register and 1 submodule
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
}
