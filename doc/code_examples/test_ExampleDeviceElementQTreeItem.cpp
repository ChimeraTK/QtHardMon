#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_ExampleDeviceElementQTreeItem

#include <QtGui/QTreeWidget>
#include <QtGui>
#include <boost/test/unit_test.hpp>
#include <mtca4u/BackendFactory.h>

#include "TestUtilities.h"

// Evil but awesome
#define private public

#include "ExampleDeviceElementQTreeItem.h"
#include "ExamplePropertiesWidget.h"

struct ExampleDeviceElementQTreeItem_fixtureBase {
  QApplication *app;
  PropertiesWidgetProvider propertiesWidgetProvider;
  ExamplePropertiesWidget *propertiesWidget;

  DeviceElementQTreeItem_fixtureBase() {
    int argc = 0;
    char **argv = nullptr;
    app = new QApplication(argc, argv);

    propertiesWidget = new ExamplePropertiesWidget(nullptr);
    propertiesWidgetProvider.registerWidget(
        ExampleDeviceElementQTreeItem::DataType, propertiesWidget, -1);
  }
};

struct DeviceAccessSetup_fixture {
  mtca4u::Device device;

  DeviceAccessSetup_fixture(const std::string &dmapFile,
                            const std::string &deviceName) {
    mtca4u::BackendFactory::getInstance().setDMapFilePath(dmapFile);
    device.open(deviceName);
  }
};

struct ExampleDeviceElementQTreeItem_fixture
    : public ExampleDeviceElementQTreeItem_fixtureBase,
      public DeviceAccessSetup_fixture {
  QTreeWidget *treeWidget;
  DeviceElementQTreeItem *item;

  ExampleDeviceElementQTreeItem_fixture(const std::string &dmapFile,
                                        const std::string &deviceName,
                                        const std::string &registerPath)
      : DeviceAccessSetup_fixture(dmapFile, deviceName) {
    const mtca4u::RegisterCatalogue registerCatalogue =
        device.getRegisterCatalogue();
    treeWidget = new QTreeWidget;
    item = new ExampleDeviceElementQTreeItem(
        device,
        registerCatalogue.getRegister(mtca4u::RegisterPath(registerPath)),
        treeWidget, propertiesWidgetProvider);
  }
};

BOOST_AUTO_TEST_CASE(ExampleDeviceElementQTreeItem_constructor) {
  ExampleDeviceElementQTreeItem_fixture fixture(
      "test_files/test_dmap.dmap", "DEVICE_NAME", "MODULE/REGISTER");

  // Check if the type returned is appropriate, so it can be recognized by
  // PropertiesWidgetProvider
  BOOST_CHECK_EQUAL(fixture.item->type(),
                    static_cast<int>(ExampleDeviceElementQTreeItem::DataType));

  // Check if the name of the item is set correctly
  BOOST_CHECK_EQUAL(fixture.item->text(0).toStdString(), "REGISTER");

  // Check if the item is added along with its corresponding module item
  BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItemCount(), 1);
  BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->text(0).toStdString(),
                    "MODULE");
  BOOST_CHECK_EQUAL(fixture.treeWidget->topLevelItem(0)->childCount(), 1);
  BOOST_CHECK_EQUAL(
      fixture.treeWidget->topLevelItem(0)->child(0)->text(0).toStdString(),
      "REGISTER");
}

BOOST_AUTO_TEST_CASE(ExampleDeviceElementQTreeItem_fillsRegisterProperties) {
  ExampleDeviceElementQTreeItem_fixture fixture(
      "test_files/test_dmap.dmap", "DEVICE_NAME", "MODULE/REGISTER");

  // Check if the entry state of ExamplePropertiesRegister is empty
  BOOST_CHECK_EQUAL(fixture.propertiesWidget->ui->registerNameDisplay->text()
                        .toStdString()
                        .c_str(),
                    "");
  BOOST_CHECK_EQUAL(
      fixture.propertiesWidget->ui->moduleDisplay->text().toStdString().c_str(),
      "");
  BOOST_CHECK_EQUAL(
      fixture.propertiesWidget->ui->registerNElementsDisplay->text()
          .toStdString()
          .c_str(),
      "");

  fixture.item->updateRegisterProperties();

  // Check if updateRegisterProperties() sets ExamplePropertiesWidget to
  // appropriate state
  BOOST_CHECK_EQUAL(fixture.propertiesWidget->ui->registerNameDisplay->text()
                        .toStdString()
                        .c_str(),
                    "REGISTER");
  BOOST_CHECK_EQUAL(
      fixture.propertiesWidget->ui->moduleDisplay->text().toStdString().c_str(),
      "MODULE");
  BOOST_CHECK_EQUAL(
      fixture.propertiesWidget->ui->registerNElementsDisplay->text()
          .toStdString()
          .c_str(),
      "1");
}

BOOST_AUTO_TEST_CASE(ExampleDeviceElementQTreeItem_ReadAndWrite) {
  ExampleDeviceElementQTreeItem_fixture fixture(
      "test_files/test_dmap.dmap", "DEVICE_NAME", "MODULE/REGISTER");

  fixture.item->updateRegisterProperties();

  fixture.item->readData();

  // Checks...

  fixture.item->writeData();

  // Checks...
}