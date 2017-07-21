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

    QtHardmon_populatesRegisterTree_fixture fixtureNumerical("test_QtHardMon_populatesRegisterTree.dmap", "NumDev");

    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItemCount(), 2);
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 3);
    BOOST_CHECK_EQUAL(fixtureNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(1)->childCount(), 2);

    QtHardmon_populatesRegisterTree_fixture fixtureNonNumerical("test_QtHardMon_populatesRegisterTree.dmap", "LmapDev");

    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItemCount(), 3);
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(0)->childCount(), 2);
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(1)->childCount(), 1);
    BOOST_CHECK_EQUAL(fixtureNonNumerical.qtHardMon._hardMonForm.registerTreeWidget->topLevelItem(2)->childCount(), 2);
}
