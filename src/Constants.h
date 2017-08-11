
#ifndef SOURCE_DIRECTORY__CONSTANTS_H_
#define SOURCE_DIRECTORY__CONSTANTS_H_

#include <qtreewidget.h>

enum class DeviceElementDataType {
    ModuleDataType = QTreeWidgetItem::UserType + 1,
    NumAddressedRegisterDataType,
    MultiplexedAreaDataType,
    SequenceRegisterDataType,
    GenericRegisterDataType
};

// Constants
namespace qthardmon {
const int FIXED_POINT_DISPLAY_COLUMN = 0;
const int HEX_VALUE_DISPLAY_COLUMN = 1;
const int FLOATING_POINT_DISPLAY_COLUMN = 2;

// FIXME: how to solve the problem of the word size? Should come from pci express.
// => need to improve the api
const size_t WORD_SIZE_IN_BYTES = 4;

}

#endif //SOURCE_DIRECTORY__CONSTANTS_H_
