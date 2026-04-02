#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestPreferencesProvider

#include "PreferencesProvider.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(TestSetAndGet) {
  PreferencesProvider provider;

  int integerValue = 5;
  provider.setValue("intVal", integerValue);

  BOOST_CHECK_EQUAL(provider.getRawValue("intVal").type(), QMetaType::Int);

  int integerGet = provider.getValue<int>("intVal");

  BOOST_CHECK_EQUAL(integerValue, integerGet);

  float floatValue = 6.5;
  provider.setValue("floatVal", floatValue);

  BOOST_CHECK_EQUAL(provider.getRawValue("floatVal").type(), QMetaType::Float);

  float floatGet = provider.getValue<float>("floatVal");

  BOOST_CHECK_EQUAL(floatValue, floatGet);

  QString stringValue = "Testing";
  provider.setValue("stringVal", stringValue);

  BOOST_CHECK_EQUAL(provider.getRawValue("stringVal").type(), QMetaType::QString);

  std::string stringGet = provider.getValue<std::string>("stringVal");

  BOOST_CHECK_EQUAL(stringValue.toStdString(), stringGet);

  bool boolValue = true;
  provider.setValue("boolVal", boolValue);

  BOOST_CHECK_EQUAL(provider.getRawValue("boolVal").type(), QMetaType::Bool);

  bool boolGet = provider.getValue<bool>("boolVal");

  BOOST_CHECK_EQUAL(boolValue, boolGet);

  double doubleValue = 6.5;
  provider.setValue("doubleVal", doubleValue);

  BOOST_CHECK_EQUAL(provider.getRawValue("doubleVal").type(), QMetaType::Double);

  double doubleGet = provider.getValue<double>("doubleVal");

  BOOST_CHECK_EQUAL(doubleValue, doubleGet);

  // This one is used to test not handling the type in getValue.

  uint uintValue = 20;
  provider.setValue("uintVal", uintValue);

  BOOST_CHECK_EQUAL(provider.getRawValue("uintVal").type(), QMetaType::UInt);

  BOOST_CHECK_THROW(provider.getValue<uint>("uintVal"), InvalidOperationException);

  BOOST_CHECK_EQUAL(provider.getRawValue("uintVal").toUInt(), uintValue);
}

BOOST_AUTO_TEST_CASE(TestThrowsAndNewValues) {
  PreferencesProvider provider;

  BOOST_CHECK_THROW(provider.getValue<int>("intVal"), InvalidOperationException);

  int integerValue = 5;
  provider.setValue("intVal", integerValue);

  float floatValue = 6.5;
  provider.setValue("floatVal", floatValue);

  QString stringValue = "Testing";
  provider.setValue("stringVal", stringValue);

  BOOST_CHECK_THROW(provider.setValue("intVal", floatValue), InvalidOperationException);
  BOOST_CHECK_THROW(provider.setValue("intVal", stringValue), InvalidOperationException);
  BOOST_CHECK_THROW(provider.setValue("stringVal", floatValue), InvalidOperationException);
  BOOST_CHECK_THROW(provider.setValue("stringVal", integerValue), InvalidOperationException);
  BOOST_CHECK_THROW(provider.setValue("floatVal", stringValue), InvalidOperationException);
  // FIXME: this should not throw, should be implicitly casted, right?
  BOOST_CHECK_THROW(provider.setValue("floatVal", integerValue), InvalidOperationException);

  int newIntegerValue = 8;
  provider.setValue("intVal", newIntegerValue);

  BOOST_CHECK_EQUAL(provider.getValue<int>("intVal"), newIntegerValue);

  float newFloatValue = 13.3;
  provider.setValue("floatVal", newFloatValue);

  BOOST_CHECK_EQUAL(provider.getValue<float>("floatVal"), newFloatValue);
}
