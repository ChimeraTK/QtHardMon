#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_RegisterPropertiesWidget

#include <boost/test/unit_test.hpp>
#include <type_traits> // std::is_base_of
#include <QtGui>

#include "TestUtilities.h"

// Evil but awesome
#define private public

#include "RegisterPropertiesWidget.h"

struct RegisterPropertiesWidget_fixtureBase {
    QApplication * app;
    RegisterPropertiesWidget * widget;
    RegisterPropertiesWidget_fixtureBase() {
        int argc = 0;
        char ** argv = nullptr;
        app = new QApplication(argc, argv);

        widget = new RegisterPropertiesWidget(NULL);
    }
};

/*
 * The window is properly filled with no data and proper states of particular window components.
*/
BOOST_AUTO_TEST_CASE ( RegisterPropertiesWidget_QWidget )
{
    bool qWidgetIsBaseOfRegisterTreeWidget = std::is_base_of<QWidget, RegisterPropertiesWidget>::value;
    BOOST_CHECK_EQUAL(qWidgetIsBaseOfRegisterTreeWidget, true);

}

/*
 * Numeric addressed register item is properly constructed and returns correct data type.
 * The item properly assigns itself to QTreeWidget.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedRegisterQTreeItem_constructor )
{
    RegisterPropertiesWidget_fixtureBase fixture;

    TestUtilities::checkRegisterProperties(fixture.widget, "", "", "", "", "", "", "", "", "");
    RegisterPropertiesWidget::RegisterProperties properties("RegName", "ModuleName", "10", "4", "4444", "3", "18", "3", "0");
    fixture.widget->setRegisterProperties(properties);
    TestUtilities::checkRegisterProperties(fixture.widget, "RegName", "ModuleName", "10", "4", "4444", "3", "18", "3", "0");

}