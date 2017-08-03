#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_DeviceElementQTreeItem

#include <boost/test/unit_test.hpp>
#include <type_traits> // std::is_base_of

// Evil but awesome
#define private public

#include "DeviceElementQTreeItem.h"

struct DeviceElementQTreeItem_fixtureBase {
    DeviceElementQTreeItem * DeviceElementQTreeItem;

    DeviceElementQTreeItem_fixtureBase() {
    }

};

/*
 * The window is properly filled with no data and proper states of particular window components.
*/
BOOST_AUTO_TEST_CASE ( DeviceElementQTreeItem_QTreeWidgetItem )
{
    bool qTreeWidgetItemIsBaseOfDeviceElementQTreeItem = std::is_base_of<QTreeWidgetItem, DeviceElementQTreeItem>::value;
    BOOST_CHECK_EQUAL(qTreeWidgetItemIsBaseOfDeviceElementQTreeItem, true);

}

