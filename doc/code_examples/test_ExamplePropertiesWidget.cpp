#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_ExamplePropertiesWidget

#include <QtGui>
#include <boost/test/unit_test.hpp>
#include <type_traits> // std::is_base_of

// Evil but awesome
#define private public

#include "ExamplePropertiesWidget.h"

struct ExamplePropertiesWidget_fixtureBase {
  QApplication* app;
  ExamplePropertiesWidget* widget;
  ExamplePropertiesWidget_fixtureBase() {
    int argc = 0;
    char** argv = nullptr;
    app = new QApplication(argc, argv);

    widget = new ExamplePropertiesWidget(NULL);
  }
};

BOOST_AUTO_TEST_CASE(ExamplePropertiesWidget_QWidget) {
  // Check if the widget is properly deriving from QWidget
  bool qWidgetIsBaseOfRegisterTreeWidget = std::is_base_of<QWidget, ExamplePropertiesWidget>::value;
  BOOST_CHECK_EQUAL(qWidgetIsBaseOfRegisterTreeWidget, true);
}

BOOST_AUTO_TEST_CASE(NumericAddressedRegisterQTreeItem_constructor) {
  ExamplePropertiesWidget_fixtureBase fixture;

  // Check if the entry state of ExamplePropertiesRegister is empty
  BOOST_CHECK_EQUAL(fixture.widget->ui->registerNameDisplay->text().toStdString().c_str(), "");
  BOOST_CHECK_EQUAL(fixture.widget->ui->moduleDisplay->text().toStdString().c_str(), "");
  BOOST_CHECK_EQUAL(fixture.widget->ui->registerNElementsDisplay->text().toStdString().c_str(), "");

  // Check if interface is properly implemented and the default values are
  // provided for unused parameters
  fixture.widget->setNames({"ModuleName", "RegName"});
  fixture.widget->setSize(3);
  fixture.widget->setAddress();
  fixture.widget->setFixedPointInfo();

  // Check if values are set correctly
  BOOST_CHECK_EQUAL(fixture.widget->ui->registerNameDisplay->text().toStdString().c_str(), "RegName");
  BOOST_CHECK_EQUAL(fixture.widget->ui->moduleDisplay->text().toStdString().c_str(), "ModuleName");
  BOOST_CHECK_EQUAL(fixture.widget->ui->registerNElementsDisplay->text().toStdString().c_str(), "3");
}
