#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE test_PreferencesProvider

#include <boost/test/unit_test.hpp>

// Evil but awesome
#define private public

#include "PreferencesProvider.h"

struct PreferencesProvider_fixtureBase {
    PreferencesProvider * provider;
    PreferencesProvider_fixtureBase() {

        provider = new PreferencesProvider();
    }
};

BOOST_AUTO_TEST_CASE ( PreferencesProvider_constructor )
{
    PreferencesProvider_fixtureBase fixture;
    BOOST_CHECK_EQUAL(fixture.provider->settings_.size(), 0);
}

BOOST_AUTO_TEST_CASE ( PreferencesProvider_setAndGet )
{
    PreferencesProvider_fixtureBase fixture;
    
    int integerValue = 5;
    fixture.provider->setValue("intVal", integerValue);

    BOOST_CHECK_EQUAL(fixture.provider->getRawValue("intVal").type(), QMetaType::Int);

    int integerGet = fixture.provider->getValue<int>("intVal");

    BOOST_CHECK_EQUAL(integerValue, integerGet);

    float floatValue = 6.5;
    fixture.provider->setValue("floatVal", floatValue);

    BOOST_CHECK_EQUAL(fixture.provider->getRawValue("floatVal").type(), QMetaType::Float);

    float floatGet = fixture.provider->getValue<float>("floatVal");

    BOOST_CHECK_EQUAL(floatValue, floatGet);

    QString stringValue = "Testing";
    fixture.provider->setValue("stringVal", stringValue);

    BOOST_CHECK_EQUAL(fixture.provider->getRawValue("stringVal").type(), QMetaType::QString);

    std::string stringGet = fixture.provider->getValue<std::string>("stringVal");

    BOOST_CHECK_EQUAL(stringValue.toStdString(), stringGet);

    bool boolValue = true;
    fixture.provider->setValue("boolVal", boolValue);

    BOOST_CHECK_EQUAL(fixture.provider->getRawValue("boolVal").type(), QMetaType::Bool);

    bool boolGet = fixture.provider->getValue<bool>("boolVal");

    BOOST_CHECK_EQUAL(boolValue, boolGet);

    double doubleValue = 6.5;
    fixture.provider->setValue("doubleVal", doubleValue);

    BOOST_CHECK_EQUAL(fixture.provider->getRawValue("doubleVal").type(), QMetaType::Double);

    double doubleGet = fixture.provider->getValue<double>("doubleVal");

    BOOST_CHECK_EQUAL(doubleValue, doubleGet);


    // This one is used to test not handling the type in getValue. 

    uint uintValue = 20;
    fixture.provider->setValue("uintVal", uintValue);

    BOOST_CHECK_EQUAL(fixture.provider->getRawValue("uintVal").type(), QMetaType::UInt);

    BOOST_CHECK_THROW(fixture.provider->getValue<uint>("uintVal"), InvalidOperationException);
    
    BOOST_CHECK_EQUAL(fixture.provider->getRawValue("uintVal").toUInt(), uintValue);
    

}

BOOST_AUTO_TEST_CASE ( PreferencesProvider_throwsAndNewValues )
{
    PreferencesProvider_fixtureBase fixture;
    
    BOOST_CHECK_THROW(fixture.provider->getValue<int>("intVal"), InvalidOperationException);

    int integerValue = 5;
    fixture.provider->setValue("intVal", integerValue);

    float floatValue = 6.5;
    fixture.provider->setValue("floatVal", floatValue);

    QString stringValue = "Testing";
    fixture.provider->setValue("stringVal", stringValue);

    BOOST_CHECK_THROW(fixture.provider->setValue("intVal", floatValue), InvalidOperationException);
    BOOST_CHECK_THROW(fixture.provider->setValue("intVal", stringValue), InvalidOperationException);
    BOOST_CHECK_THROW(fixture.provider->setValue("stringVal", floatValue), InvalidOperationException);
    BOOST_CHECK_THROW(fixture.provider->setValue("stringVal", integerValue), InvalidOperationException);
    BOOST_CHECK_THROW(fixture.provider->setValue("floatVal", stringValue), InvalidOperationException);
    // FIXME: this should not throw, should be implicitly casted, right?
    BOOST_CHECK_THROW(fixture.provider->setValue("floatVal", integerValue), InvalidOperationException);


    int newIntegerValue = 8;
    fixture.provider->setValue("intVal", newIntegerValue);

    BOOST_CHECK_EQUAL(fixture.provider->getValue<int>("intVal"), newIntegerValue);

    float newFloatValue = 13.3;
    fixture.provider->setValue("floatVal", newFloatValue);

    BOOST_CHECK_EQUAL(fixture.provider->getValue<float>("floatVal"), newFloatValue);

}

