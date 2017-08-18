#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_DeviceElementQTreeItem

#include <boost/test/unit_test.hpp>
#include <type_traits> // std::is_base_of
#include <QtGui>
#include <mtca4u/BackendFactory.h>
#include <QtGui/QTreeWidget>

#include "TestUtilities.h"

// Evil but awesome
#define private public

#include "DeviceElementQTreeItem.h"
#include "RegisterPropertiesWidget.h"
#include "ModulePropertiesWidget.h"
#include "GenericRegisterPropertiesWidget.h"

struct DeviceElementQTreeItem_fixtureBase {
    QApplication * app;
    PropertiesWidgetProvider propertiesWidgetProvider;
    RegisterPropertiesWidget * propertiesWidget;
    ModulePropertiesWidget * modulePropertiesWidget;
    GenericRegisterPropertiesWidget * genericRegisterPropertiesWidget;

    DeviceElementQTreeItem_fixtureBase() {
        int argc = 0;
        char ** argv = nullptr;
        app = new QApplication(argc, argv);

        propertiesWidget = new RegisterPropertiesWidget(nullptr);
        modulePropertiesWidget = new ModulePropertiesWidget(nullptr);
        genericRegisterPropertiesWidget = new GenericRegisterPropertiesWidget(nullptr);
        propertiesWidgetProvider.registerWidget(DeviceElementDataType::ModuleDataType, modulePropertiesWidget, -1);
        propertiesWidgetProvider.registerWidget(DeviceElementDataType::NumAddressedRegisterDataType, propertiesWidget, -1);
        propertiesWidgetProvider.registerWidget(DeviceElementDataType::MultiplexedAreaDataType, propertiesWidget, -1);
        propertiesWidgetProvider.registerWidget(DeviceElementDataType::SequenceRegisterDataType, propertiesWidget, -1);
        propertiesWidgetProvider.registerWidget(DeviceElementDataType::GenericRegisterDataType, genericRegisterPropertiesWidget, -1);
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
    DeviceElementQTreeItem_fixtureBase fixture;
    QTreeWidgetItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL, fixture.propertiesWidgetProvider);
    BOOST_CHECK_EQUAL(moduleItem->type(), static_cast<int>(DeviceElementDataType::ModuleDataType));
}

/*
 * Mdule item cannot be used to read or write.
*/
BOOST_AUTO_TEST_CASE ( ModuleQTreeItem_ReadAndWriteThrows )
{
    DeviceElementQTreeItem_fixtureBase fixture;
    DeviceElementQTreeItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL, fixture.propertiesWidgetProvider);
    BOOST_CHECK_THROW(moduleItem->readData(), InvalidOperationException);
    BOOST_CHECK_THROW(moduleItem->writeData(), InvalidOperationException);
}

/*
 * Module item properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( ModuleQTreeItem_fillsRegisterProperties )
{
    DeviceElementQTreeItem_fixtureBase fixture;
    DeviceElementQTreeItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL, fixture.propertiesWidgetProvider);
    TestUtilities::checkModuleProperties(fixture.modulePropertiesWidget, "", "");
    moduleItem->updateRegisterProperties();

    // FIXME: this test does not determine if the properties widget is properly filled if something was set in between.
    TestUtilities::checkModuleProperties(fixture.modulePropertiesWidget, "testing", "0");

    // DeviceElementQTreeItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL, RegisterpropertiesWidgetProvider);


}

#include "RegisterQTreeItem.h"


struct DeviceAccessSetup_fixture {
    mtca4u::Device device;

    DeviceAccessSetup_fixture(const std::string & dmapFile, const std::string & deviceName) {
        mtca4u::BackendFactory::getInstance().setDMapFilePath(dmapFile);
        device.open(deviceName);
    }
};

struct RegisterQTreeItem_fixture : public DeviceAccessSetup_fixture, public DeviceElementQTreeItem_fixtureBase {
    QTreeWidget * treeWidget;
    DeviceElementQTreeItem * registerQTreeItem;
    mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor;

    RegisterQTreeItem_fixture(const std::string & dmapFile, const std::string & deviceName, const std::string & registerPath, int initialValue) :
    DeviceAccessSetup_fixture(dmapFile, deviceName),
    oneDRegisterAccessor(device.getOneDRegisterAccessor<double>(mtca4u::RegisterPath(registerPath)))
    {
        const mtca4u::RegisterCatalogue registerCatalogue = device.getRegisterCatalogue();
        treeWidget = new QTreeWidget;
        boost::shared_ptr<mtca4u::RegisterInfo> regInfo = registerCatalogue.getRegister(mtca4u::RegisterPath(registerPath));
        registerQTreeItem = new RegisterQTreeItem(device, registerCatalogue.getRegister(mtca4u::RegisterPath(registerPath)), treeWidget, propertiesWidgetProvider);
        oneDRegisterAccessor.read();
        oneDRegisterAccessor[0] = initialValue;
        oneDRegisterAccessor.write();
        oneDRegisterAccessor.read();
    }

};

BOOST_AUTO_TEST_CASE ( RegisterQTreeItem_constructor )
{
    RegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV", "APP0/MODULE0", 5.0);
    BOOST_CHECK_EQUAL(fixture.registerQTreeItem->type(), static_cast<int>(DeviceElementDataType::GenericRegisterDataType));
    BOOST_CHECK_EQUAL(fixture.registerQTreeItem->text(0).toStdString(), "MODULE0");
    
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItemCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->text(0).toStdString(), "APP0");
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->childCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->child(0)->text(0).toStdString(), "MODULE0");
}



/*
 * Numeric addressed register properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( RegisterQTreeItem_fillsRegisterProperties )
{
    RegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV", "APP0/MODULE0", 5.0);
    
    TestUtilities::checkRegisterProperties(fixture.genericRegisterPropertiesWidget, "", "", "");
    fixture.registerQTreeItem->updateRegisterProperties();

    TestUtilities::checkRegisterProperties(fixture.genericRegisterPropertiesWidget, "MODULE0", "APP0", "2");
}

/*
 * Numeric addressed register reads from / writes to device properly.
*/
BOOST_AUTO_TEST_CASE ( RegisterQTreeItem_ReadAndWrite )
{
    RegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV", "APP0/MODULE0", 5.0);
    
    fixture.registerQTreeItem->updateRegisterProperties();

    fixture.registerQTreeItem->readData();

    TestUtilities::checkTableData(fixture.genericRegisterPropertiesWidget, {std::make_tuple(5, 5, 5.0), std::make_tuple(0, 0, 0.0)});

    TestUtilities::setTableValue(fixture.genericRegisterPropertiesWidget, 1, 0, std::make_tuple(3, 3, 3.0));
    
    fixture.registerQTreeItem->writeData();

    fixture.oneDRegisterAccessor.read();

    BOOST_CHECK_EQUAL(fixture.oneDRegisterAccessor[1], 3.0);

}

/*
 * Numeric addressed register properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( RegisterQTreeItem_fillsRegisterPropertiesNonNumerical )
{
    RegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy_lmap.dmap", "LMAPDEV", "FullArea", 5.0);
    
    TestUtilities::checkRegisterProperties(fixture.genericRegisterPropertiesWidget, "", "", "");
    fixture.registerQTreeItem->updateRegisterProperties();

    TestUtilities::checkRegisterProperties(fixture.genericRegisterPropertiesWidget, "FullArea", "", "10");
}

/*
 * Numeric addressed register reads from / writes to device properly.
*/
BOOST_AUTO_TEST_CASE ( RegisterQTreeItem_ReadAndWriteNonNumerical )
{
    RegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy_lmap.dmap", "LMAPDEV", "FullArea", 5.0);
    
    fixture.registerQTreeItem->updateRegisterProperties();

    fixture.registerQTreeItem->readData();

    TestUtilities::checkTableData(fixture.genericRegisterPropertiesWidget, {std::make_tuple(5, 5, 5.0), std::make_tuple(0, 0, 0.0),
    std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0),
    std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0), std::make_tuple(0, 0, 0.0)});

    TestUtilities::setTableValue(fixture.genericRegisterPropertiesWidget, 1, 0, std::make_tuple(3, 3, 3.0));
    
    fixture.registerQTreeItem->writeData();

    fixture.oneDRegisterAccessor.read();

    BOOST_CHECK_EQUAL(fixture.oneDRegisterAccessor[1], 3.0);

}

#include "NumericAddressedRegisterQTreeItem.h"

struct NumericAddressedRegisterQTreeItem_fixture : public DeviceAccessSetup_fixture, public DeviceElementQTreeItem_fixtureBase {
    QTreeWidget * treeWidget;
    DeviceElementQTreeItem * numericAddressedRegisterQTreeItem;
    mtca4u::OneDRegisterAccessor<double> oneDRegisterAccessor;

    NumericAddressedRegisterQTreeItem_fixture(const std::string & dmapFile, const std::string & deviceName, const std::string & registerPath, int initialValue) :
    DeviceAccessSetup_fixture(dmapFile, deviceName),
    oneDRegisterAccessor(device.getOneDRegisterAccessor<double>(mtca4u::RegisterPath(registerPath)))
    {
        const mtca4u::RegisterCatalogue registerCatalogue = device.getRegisterCatalogue();
        treeWidget = new QTreeWidget;
        numericAddressedRegisterQTreeItem = new NumericAddressedRegisterQTreeItem(device, registerCatalogue.getRegister(mtca4u::RegisterPath(registerPath)), treeWidget, propertiesWidgetProvider);
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
 * Numeric addressed register properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( NumericAddressedRegisterQTreeItem_fillsRegisterProperties )
{
    NumericAddressedRegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV", "APP0/MODULE0", 5.0);
    
    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "", "", "", "", "", "", "", "", "");
    fixture.numericAddressedRegisterQTreeItem->updateRegisterProperties();

    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "MODULE0", "APP0", "1", "16", "2", "8", "32", "0", "1");

    // DeviceElementQTreeItem * moduleItem = new ModuleQTreeItem("testing", (QTreeWidget *) NULL, RegisterPropertiesWidget);
}

/*
 * Numeric addressed register reads from / writes to device properly.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedRegisterQTreeItem_ReadAndWrite )
{
    NumericAddressedRegisterQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV", "APP0/MODULE0", 5.0);
    
    fixture.numericAddressedRegisterQTreeItem->updateRegisterProperties();

    fixture.numericAddressedRegisterQTreeItem->readData();

    TestUtilities::checkTableData(fixture.propertiesWidget, {std::make_tuple(5, 5, 5.0), std::make_tuple(3, 30, 3.0)});

    TestUtilities::setTableValue(fixture.propertiesWidget, 1, 0, std::make_tuple(4, 4, 4.0));

    fixture.numericAddressedRegisterQTreeItem->writeData();

    fixture.oneDRegisterAccessor.read();

    BOOST_CHECK_EQUAL(fixture.oneDRegisterAccessor[1], 4.0);

}

#include "NumericAddressedMultiplexedAreaQTreeItem.h"

struct NumericAddressedMultiplexedAreaQTreeItem_fixture : public DeviceAccessSetup_fixture, public DeviceElementQTreeItem_fixtureBase {
    QTreeWidget * treeWidget;
    DeviceElementQTreeItem * numericAddressedMultiplexedAreaQTreeItem;
    mtca4u::TwoDRegisterAccessor<double> twoDRegisterAccessor;

    NumericAddressedMultiplexedAreaQTreeItem_fixture(const std::string & dmapFile, const std::string & deviceName, const std::string & registerPath, int initialValue) :
    DeviceAccessSetup_fixture(dmapFile, deviceName),
    twoDRegisterAccessor(device.getTwoDRegisterAccessor<double>(mtca4u::RegisterPath(registerPath)))
    {
        const mtca4u::RegisterCatalogue registerCatalogue = device.getRegisterCatalogue();
        treeWidget = new QTreeWidget;
        mtca4u::RegisterCatalogue::const_iterator firstSequenceItem = registerCatalogue.begin();
        
        // Iterating to actual first sequence item of APP0/AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA, FIXME: don't hardcode it.
        for (int i = 0; i < 20; ++i) {
            ++firstSequenceItem;
        }
        
        numericAddressedMultiplexedAreaQTreeItem = new NumericAddressedMultiplexedAreaQTreeItem(device, registerCatalogue.getRegister(mtca4u::RegisterPath(registerPath)), registerCatalogue, firstSequenceItem, treeWidget, propertiesWidgetProvider);
        twoDRegisterAccessor.read();
        twoDRegisterAccessor[0][0] = initialValue;
        twoDRegisterAccessor.write();
        twoDRegisterAccessor.read();
    }

};

/*
 * Numeric addressed register item is properly constructed and returns correct data type.
 * The item properly assigns itself to QTreeWidget.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedMultiplexedAreaQTreeItem_constructor )
{
    NumericAddressedMultiplexedAreaQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT", "APP0/AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA", 5.0);
    BOOST_CHECK_EQUAL(fixture.numericAddressedMultiplexedAreaQTreeItem->type(), static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType));
    BOOST_CHECK_EQUAL(fixture.numericAddressedMultiplexedAreaQTreeItem->text(0).toStdString(), "AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA");
    
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItemCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->text(0).toStdString(), "APP0");
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->childCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->child(0)->text(0).toStdString(), "AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA");
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->child(0)->childCount(), 16);
}

#include "NumericAddressedSequenceRegisterQTreeItem.h"

/*
 * Numeric addressed register properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( NumericAddressedMultiplexedAreaQTreeItem_fillsRegisterProperties )
{
    NumericAddressedMultiplexedAreaQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT", "APP0/AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA", 5.0);
    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "", "", "", "", "", "", "", "", "");
    fixture.numericAddressedMultiplexedAreaQTreeItem->updateRegisterProperties();
    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA", "APP0", "13", "1000", "", "212992", "", "", "");
    NumericAddressedSequenceRegisterQTreeItem * childItem = dynamic_cast<NumericAddressedSequenceRegisterQTreeItem *>(fixture.numericAddressedMultiplexedAreaQTreeItem->child(0));
    
    if (!childItem) {
        BOOST_FAIL("QTreeWidgetItem not casted properly");
    } else {
        childItem->updateRegisterProperties();
    }

    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "SEQUENCE_DAQ0_ADCA_0", "APP0", "13", "1000", "1", "4", "32", "0", "1");
}

/*
 * Numeric addressed register reads from / writes to device properly.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedMultiplexedAreaQTreeItem_ReadAndWrite )
{
    NumericAddressedMultiplexedAreaQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT", "APP0/AREA_MULTIPLEXED_SEQUENCE_DAQ0_ADCA", 0.0);
    
    fixture.numericAddressedMultiplexedAreaQTreeItem->updateRegisterProperties();

    BOOST_CHECK_THROW(fixture.numericAddressedMultiplexedAreaQTreeItem->readData(), InvalidOperationException);
    BOOST_CHECK_THROW(fixture.numericAddressedMultiplexedAreaQTreeItem->writeData(), InvalidOperationException);

    NumericAddressedSequenceRegisterQTreeItem * childItem = dynamic_cast<NumericAddressedSequenceRegisterQTreeItem *>(fixture.numericAddressedMultiplexedAreaQTreeItem->child(0));
    
    if (!childItem) {
        BOOST_FAIL("QTreeWidgetItem not casted properly");
    } else {
        childItem->updateRegisterProperties();
    }

    childItem->readData();

    TestUtilities::checkTableData(fixture.propertiesWidget, {std::make_tuple(0, 0, 0.0)}, 4096);

    TestUtilities::setTableValue(fixture.propertiesWidget, 0, 0, std::make_tuple(3, 3, 3.0));

    childItem->writeData();

    fixture.twoDRegisterAccessor.read();

    BOOST_CHECK_EQUAL(fixture.twoDRegisterAccessor[0][0], 3.0);

}

#include "NumericAddressedCookedMultiplexedAreaQTreeItem.h"

struct NumericAddressedCookedMultiplexedAreaQTreeItem_fixture : public DeviceAccessSetup_fixture, public DeviceElementQTreeItem_fixtureBase {
    QTreeWidget * treeWidget;
    DeviceElementQTreeItem * numericAddressedCookedMultiplexedAreaQTreeItem;
    mtca4u::TwoDRegisterAccessor<double> twoDRegisterAccessor;

    NumericAddressedCookedMultiplexedAreaQTreeItem_fixture(const std::string & dmapFile, const std::string & deviceName, const std::string & registerPath, int initialValue) :
    DeviceAccessSetup_fixture(dmapFile, deviceName),
    twoDRegisterAccessor(device.getTwoDRegisterAccessor<double>(mtca4u::RegisterPath(registerPath)))
    {
        const mtca4u::RegisterCatalogue registerCatalogue = device.getRegisterCatalogue();
        treeWidget = new QTreeWidget;
        
        numericAddressedCookedMultiplexedAreaQTreeItem = new NumericAddressedCookedMultiplexedAreaQTreeItem(device, registerCatalogue.getRegister(mtca4u::RegisterPath(registerPath)), treeWidget, propertiesWidgetProvider);
        twoDRegisterAccessor.read();
        twoDRegisterAccessor[0][0] = initialValue;
        twoDRegisterAccessor.write();
        twoDRegisterAccessor.read();
    }

};

/*
 * Numeric addressed register item is properly constructed and returns correct data type.
 * The item properly assigns itself to QTreeWidget.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedCookedMultiplexedAreaQTreeItem_constructor )
{
    NumericAddressedCookedMultiplexedAreaQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT", "APP0/DAQ0_ADCA", 5.0);
    BOOST_CHECK_EQUAL(fixture.numericAddressedCookedMultiplexedAreaQTreeItem->type(), static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType));
    BOOST_CHECK_EQUAL(fixture.numericAddressedCookedMultiplexedAreaQTreeItem->text(0).toStdString(), "DAQ0_ADCA");
    
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItemCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->text(0).toStdString(), "APP0");
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->childCount(), 1);
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->child(0)->text(0).toStdString(), "DAQ0_ADCA");
    BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->child(0)->childCount(), 16);
}

#include "NumericAddressedCookedSequenceRegisterQTreeItem.h"

/*
 * Numeric addressed register properly fills register properties.
 */
BOOST_AUTO_TEST_CASE ( NumericAddressedCookedMultiplexedAreaQTreeItem_fillsRegisterProperties )
{
    NumericAddressedCookedMultiplexedAreaQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT", "APP0/DAQ0_ADCA", 5.0);
    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "", "", "", "", "", "", "", "", "");
    fixture.numericAddressedCookedMultiplexedAreaQTreeItem->updateRegisterProperties();
    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "DAQ0_ADCA", "APP0", "13", "1000", "", "212992", "", "", "");
    NumericAddressedCookedSequenceRegisterQTreeItem * childItem = dynamic_cast<NumericAddressedCookedSequenceRegisterQTreeItem *>(fixture.numericAddressedCookedMultiplexedAreaQTreeItem->child(0));
    
    if (!childItem) {
        BOOST_FAIL("QTreeWidgetItem not casted properly");
    } else {
        childItem->updateRegisterProperties();
    }

    TestUtilities::checkRegisterProperties(fixture.propertiesWidget, "DAQ0_ADCA", "APP0", "13", "1000", "", "212992", "", "", "");
}

/*
 * Numeric addressed register reads from / writes to device properly.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedCookedMultiplexedAreaQTreeItem_ReadAndWrite )
{
    NumericAddressedCookedMultiplexedAreaQTreeItem_fixture fixture("test_files/test_QtHardMon_valid_dummy.dmap", "NUMDEV_MULT", "APP0/DAQ0_ADCA", 0.0);
    
    fixture.numericAddressedCookedMultiplexedAreaQTreeItem->updateRegisterProperties();

    BOOST_CHECK_THROW(fixture.numericAddressedCookedMultiplexedAreaQTreeItem->readData(), InvalidOperationException);
    BOOST_CHECK_THROW(fixture.numericAddressedCookedMultiplexedAreaQTreeItem->writeData(), InvalidOperationException);

    NumericAddressedCookedSequenceRegisterQTreeItem * childItem = dynamic_cast<NumericAddressedCookedSequenceRegisterQTreeItem *>(fixture.numericAddressedCookedMultiplexedAreaQTreeItem->child(0));
    
    if (!childItem) {
        BOOST_FAIL("QTreeWidgetItem not casted properly");
    } else {
        childItem->updateRegisterProperties();
    }

    childItem->readData();

    TestUtilities::checkTableData(fixture.propertiesWidget, {std::make_tuple(0, 0, 0.0)}, 4096);

    TestUtilities::setTableValue(fixture.propertiesWidget, 0, 0, std::make_tuple(3, 3, 3.0));

    childItem->writeData();

    fixture.twoDRegisterAccessor.read();

    BOOST_CHECK_EQUAL(fixture.twoDRegisterAccessor[0][0], 3.0);

}