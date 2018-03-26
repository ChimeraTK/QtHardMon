#include "RegisterTypeAbstractorRawImpl.h"

template<>
QVariant dataToQVariant<std::string>(std::string data){
  return QVariant(data.c_str());
}
