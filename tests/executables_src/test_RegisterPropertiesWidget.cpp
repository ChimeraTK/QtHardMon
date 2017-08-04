#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_RegisterPropertiesWidget

#include <boost/test/unit_test.hpp>
#include <type_traits> // std::is_base_of
#include <QtGui>

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

void checkRegisterProperties(RegisterPropertiesWidget * widget,
                             const std::string & registerName,
                             const std::string & moduleName,
                             const std::string & registerBar,
                             const std::string & registerNElements,
                             const std::string & registerAddress,
                             const std::string & registerSize,
                             const std::string & registerWidth,
                             const std::string & registerFracBits,
                             const std::string & registerSignBit
) {
    BOOST_CHECK_EQUAL(widget->ui->registerNameDisplay->text().toStdString().c_str(), registerName);
    BOOST_CHECK_EQUAL(widget->ui->moduleDisplay->text().toStdString().c_str(), moduleName);
    BOOST_CHECK_EQUAL(widget->ui->registerBarDisplay->text().toStdString().c_str(), registerBar);
    BOOST_CHECK_EQUAL(widget->ui->registerAddressDisplay->text().toStdString().c_str(), registerAddress);
    BOOST_CHECK_EQUAL(widget->ui->registerNElementsDisplay->text().toStdString().c_str(), registerNElements);
    BOOST_CHECK_EQUAL(widget->ui->registerSizeDisplay->text().toStdString().c_str(), registerSize);
    BOOST_CHECK_EQUAL(widget->ui->registerWidthDisplay->text().toStdString().c_str(), registerWidth);
    BOOST_CHECK_EQUAL(widget->ui->registerFracBitsDisplay->text().toStdString().c_str(), registerFracBits);
    BOOST_CHECK_EQUAL(widget->ui->registeSignBitDisplay->text().toStdString().c_str(), registerSignBit);
}

/*
 * Numeric addressed register item is properly constructed and returns correct data type.
 * The item properly assigns itself to QTreeWidget.
*/
BOOST_AUTO_TEST_CASE ( NumericAddressedRegisterQTreeItem_constructor )
{
    RegisterPropertiesWidget_fixtureBase fixture;

    checkRegisterProperties(fixture.widget, "", "", "", "", "", "", "", "", "");

    fixture.widget->setRegisterProperties("RegName", "ModuleName", new int(10), new int(4), new int(4444), new int(3), new int(18), new int(3), new int(0));
    checkRegisterProperties(fixture.widget, "RegName", "ModuleName", "10", "4", "4444", "3", "18", "3", "0");

}