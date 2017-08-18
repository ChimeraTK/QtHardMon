#include <boost/test/unit_test.hpp>
#include "TestUtilities.h"



void TestUtilities::checkRegisterProperties(RegisterPropertiesWidget * widget,
                             const std::string & registerName,
                             const std::string & moduleName,
                             const std::string & registerBar,
                             const std::string & registerAddress,
                             const std::string & registerNElements,
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

void TestUtilities::checkModuleProperties(ModulePropertiesWidget * widget,
                             const std::string & moduleName,
                             const std::string & registerNElements
) {
    BOOST_CHECK_EQUAL(widget->ui->moduleDisplay->text().toStdString().c_str(), moduleName);
    BOOST_CHECK_EQUAL(widget->ui->registerNElementsDisplay->text().toStdString().c_str(), registerNElements);
}

void TestUtilities::checkRegisterProperties(GenericRegisterPropertiesWidget * widget,
                             const std::string & registerName,
                             const std::string & moduleName,
                             const std::string & registerNElements
) {
    BOOST_CHECK_EQUAL(widget->ui->registerNameDisplay->text().toStdString().c_str(), registerName);
   BOOST_CHECK_EQUAL(widget->ui->moduleDisplay->text().toStdString().c_str(), moduleName);
   BOOST_CHECK_EQUAL(widget->ui->registerNElementsDisplay->text().toStdString().c_str(), registerNElements);
}

void TestUtilities::checkTableData(RegisterPropertiesWidget * widget,
                    std::vector<std::tuple<int, int, double>> tableDataValues,
                    int size
) {
    if (size == 0) {
        BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->rowCount(), tableDataValues.size());
        
        for (uint i = 0; i < tableDataValues.size(); ++i) {
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 0)->text().toInt(), std::get<0>(tableDataValues.at(i)));
        //    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 1)->text().toInt(), std::get<1>(tableDataValues.at(i)));
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 2)->text().toDouble(), std::get<2>(tableDataValues.at(i)));
        }
    } else {
        BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->rowCount(), size);
        
        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 0)->text().toInt(), std::get<0>(tableDataValues.at(0)));
        //    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 1)->text().toInt(), std::get<1>(tableDataValues.at(i)));
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 2)->text().toDouble(), std::get<2>(tableDataValues.at(0)));
        }
    }
    
}

// FIXME: This is not working properly - the code invocation does not make the second column (raw hex) be filled.
// Left for now, but has to be solved.
void TestUtilities::setTableValue(RegisterPropertiesWidget * widget, int row, int column,
                    std::tuple<int, int, double> dataTuple
) {
    switch (column) {
        case 0: {
            widget->ui->valuesTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(std::get<0>(dataTuple))));
            break;
        //} case 1: {
        //    widget->ui->valuesTableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(std::get<1>(dataTuple))));
        //    break;
        } case 2: {
            widget->ui->valuesTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(std::get<2>(dataTuple))));
            break;
        } default:
            BOOST_FAIL("Only values 0 or 2 accepted, check test case source code!");
    }
    // bool ok;
    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(row, 0)->text().toInt(), std::get<0>(dataTuple));
    // BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(row, 1)->text().remove(0,2).toUInt(&ok, 16), std::get<1>(dataTuple));
    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(row, 2)->text().toDouble(), std::get<2>(dataTuple));
    
}

void TestUtilities::checkTableData(GenericRegisterPropertiesWidget * widget,
                    std::vector<std::tuple<int, int, double>> tableDataValues,
                    int size
) {
    if (size == 0) {
        BOOST_REQUIRE_EQUAL(widget->ui->valuesTableWidget->rowCount(), tableDataValues.size());
        
        for (uint i = 0; i < widget->ui->valuesTableWidget->rowCount(); ++i) {
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 0)->text().toInt(), std::get<0>(tableDataValues.at(i)));
        //    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 1)->text().toInt(), std::get<1>(tableDataValues.at(i)));
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 2)->text().toDouble(), std::get<2>(tableDataValues.at(i)));
        }
    } else {
        BOOST_REQUIRE_EQUAL(widget->ui->valuesTableWidget->rowCount(), size);
        
        for (int i = 0; i < widget->ui->valuesTableWidget->rowCount(); ++i) {
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 0)->text().toInt(), std::get<0>(tableDataValues.at(0)));
        //    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 1)->text().toInt(), std::get<1>(tableDataValues.at(i)));
            BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(i, 2)->text().toDouble(), std::get<2>(tableDataValues.at(0)));
        }
    }
    
}

// FIXME: This is not working properly - the code invocation does not make the second column (raw hex) be filled.
// Left for now, but has to be solved.
void TestUtilities::setTableValue(GenericRegisterPropertiesWidget * widget, int row, int column,
                    std::tuple<int, int, double> dataTuple
) {
    switch (column) {
        case 0: {
            widget->ui->valuesTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(std::get<0>(dataTuple))));
            break;
        //} case 1: {
        //    widget->ui->valuesTableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(std::get<1>(dataTuple))));
        //    break;
        } case 2: {
            widget->ui->valuesTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(std::get<2>(dataTuple))));
            break;
        } default:
            BOOST_FAIL("Only values 0 or 2 accepted, check test case source code!");
    }
    // bool ok;
    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(row, 0)->text().toInt(), std::get<0>(dataTuple));
    // BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(row, 1)->text().remove(0,2).toUInt(&ok, 16), std::get<1>(dataTuple));
    BOOST_CHECK_EQUAL(widget->ui->valuesTableWidget->item(row, 2)->text().toDouble(), std::get<2>(dataTuple));
    
}