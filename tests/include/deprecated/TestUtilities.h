#ifndef TESTUTILITIES_H
#define TESTUTILITIES_H

#include "GenericRegisterPropertiesWidget.h"
#include "ModulePropertiesWidget.h"
#include "RegisterPropertiesWidget.h"

class TestUtilities {
 public:
  static void checkRegisterProperties(RegisterPropertiesWidget* widget, const std::string& registerName,
      const std::string& moduleName, const std::string& registerBar, const std::string& registerAddress,
      const std::string& registerNElements, const std::string& registerSize, const std::string& registerWidth,
      const std::string& registerFracBits, const std::string& registerSignBit);

  static void checkModuleProperties(ModulePropertiesWidget* widget,
      const std::string& moduleName,
      const std::string& registerNElements);

  static void checkRegisterProperties(GenericRegisterPropertiesWidget* widget,
      const std::string& registerName,
      const std::string& moduleName,
      const std::string& registerNElements);

  static void setTableValue(
      RegisterPropertiesWidget* widget, int row, int column, std::tuple<int, int, double> dataTuple);

  static void checkTableData(RegisterPropertiesWidget* widget,
      std::vector<std::tuple<int, int, double>>
          tableDataValues,
      int size = 0);

  static void setTableValue(
      GenericRegisterPropertiesWidget* widget, int row, int column, std::tuple<int, int, double> dataTuple);

  static void checkTableData(GenericRegisterPropertiesWidget* widget,
      std::vector<std::tuple<int, int, double>>
          tableDataValues,
      int size = 0);
};

#endif // TESTUTILITIES_H
