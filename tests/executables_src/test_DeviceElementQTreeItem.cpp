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

#include "ModuleQTreeItem.h"
#include "Exceptions.h"

/*
 * The module item is properly constructed and returns correct data type.
*/
BOOST_AUTO_TEST_CASE ( ModuleQTreeItem_DataType )
{
    QTreeWidgetItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL);
    BOOST_CHECK_EQUAL(moduleItem->type(), static_cast<int>(DeviceElementDataType::ModuleDataType));
}

/*
 * The module item cannot be used to read or write.
*/
BOOST_AUTO_TEST_CASE ( ModuleQTreeItem_ReadAndWriteThrows )
{
    DeviceElementQTreeItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL);
    BOOST_CHECK_THROW(moduleItem->read(), InvalidOperationException);
    BOOST_CHECK_THROW(moduleItem->write(), InvalidOperationException);
}

/*
 * Module item properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( ModuleQTreeItem_fillsRegisterProperties )
{
    // RegisterPropertiesWidget * registerPropertiesWidget = new RegisterPropertiesWidget;
    // DeviceElementQTreeItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL, RegisterPropertiesWidget);
}