#include "RegisterTypeAbstractorImpl.h"

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const{
  return QString(_accessor[channelIndex][elementIndex].c_str());
}

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const{
  // String as hex does not make sense. We could throw, or just return an invalid variant.
  return QVariant();
}

template<>
bool RegisterTypeAbstractorImpl<std::string>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data){
  if (! data.canConvert<QString>() ){
    return false;
  }
  _accessor[channelIndex][elementIndex] = data.value<QString>().toStdString();
  return true;
}
