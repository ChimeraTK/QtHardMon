#include "RegisterTypeAbstractorRawImpl.h"

template<>
QVariant dataToQVariant<std::string>(std::string data){
  return QVariant(data.c_str());
}

template<>
QVariant dataToQVariant<int64_t>(int64_t data){
  return qlonglong(data);
}

template<>
QVariant dataToQVariant<uint64_t>(uint64_t data){
  return qulonglong(data);
}
