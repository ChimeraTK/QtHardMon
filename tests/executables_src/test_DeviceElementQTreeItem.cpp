#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_DeviceElementQTreeItem

#include <boost/test/unit_test.hpp>
#include <type_traits> // std::is_base_of
#include <QtGui>
#include <mtca4u/BackendFactory.h>
#include <QtGui/QTreeWidget>

// Evil but awesome
#define private public

#include "DeviceElementQTreeItem.h"

struct DeviceElementQTreeItem_fixtureBase {
    QApplication * app;

    DeviceElementQTreeItem_fixtureBase() {
        int argc = 0;
        char ** argv = nullptr;
        app = new QApplication(argc, argv);
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
 * Module item is properly constructed and returns correct data type.
*/
BOOST_AUTO_TEST_CASE ( ModuleQTreeItem_DataType )
{
    QTreeWidgetItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL);
    BOOST_CHECK_EQUAL(moduleItem->type(), static_cast<int>(DeviceElementDataType::ModuleDataType));
}

/*
 * Mdule item cannot be used to read or write.
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

#include "NumericAddressedRegisterQTreeItem.h"

struct DeviceAccessSetup_fixture {
    mtca4u::Device device;

    DeviceAccessSetup_fixture(const std::string & dmapFile, const std::string & deviceName) {
        mtca4u::BackendFactory::getInstance().setDMapFilePath(dmapFile);
        device.open(deviceName);
    }
};

struct NumericAddressedRegisterQTreeItem_fixture : public DeviceAccessSetup_fixture, public DeviceElementQTreeItem_fixtureBase {
    QTreeWidget * treeWidget;
    DeviceElementQTreeItem * numericAddressedRegisterQTreeItem;
    mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor;
    double * transactionVariable = new double;

    NumericAddressedRegisterQTreeItem_fixture(const std::string & dmapFile, const std::string & deviceName, const std::string & registerPath, int initialValue) :
    DeviceAccessSetup_fixture(dmapFile, deviceName),
    oneDRegisterAccessor(device.getOneDRegisterAccessor<double>(mtca4u::RegisterPath(registerPath)))
    {
        const mtca4u::RegisterCatalogue registerCatalogue = device.getRegisterCatalogue();
        treeWidget = new QTreeWidget;
        numericAddressedRegisterQTreeItem = new NumericAddressedRegisterQTreeItem(device, registerCatalogue.getRegister(mtca4u::RegisterPath(registerPath)), treeWidget, transactionVariable);
    
        oneDRegisterAccessor.read();
        oneDRegisterAccessor[0] = initialValue;
        oneDRegisterAccessor.write();
        oneDRegisterAccessor.read();
    }

};

/*
 * Numeric addressed register item is properly constructed and returns correct data type.
 * The item properly assigns itself to QTreeWidget.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedRegisterQTreeItem_constructor )
{
    NumericAddressedRegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV", "APP0/MODULE0", 5.0);
    BOOST_CHECK_EQUAL(fixture.numericAddressedRegisterQTreeItem->type(), static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType));
    BOOST_CHECK_EQUAL(fixture.numericAddressedRegisterQTreeItem->text(0).toStdString(), "MODULE0");
    
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItemCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->text(0).toStdString(), "APP0");
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->childCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->child(0)->text(0).toStdString(), "MODULE0");
}

/*
 * Numeric addressed register reads from / writes to device properly.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedRegisterQTreeItem_ReadAndWrite )
{
    NumericAddressedRegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV", "APP0/MODULE0", 5.0);
    
    fixture.numericAddressedRegisterQTreeItem->read();

    BOOST_CHECK_EQUAL(5.0, *(fixture.transactionVariable));

    *(fixture.transactionVariable) = 8.0;

    fixture.numericAddressedRegisterQTreeItem->write();

    fixture.oneDRegisterAccessor.read();

    BOOST_CHECK_EQUAL(fixture.oneDRegisterAccessor[0], 8.0);

}

/*
 * Numeric addressed register properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( NumericAddressedRegisterQTreeItem_fillsRegisterProperties )
{
    // RegisterPropertiesWidget * registerPropertiesWidget = new RegisterPropertiesWidget;
    // DeviceElementQTreeItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL, RegisterPropertiesWidget);
}
