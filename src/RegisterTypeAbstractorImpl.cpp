#include "RegisterTypeAbstractorImpl.h"

template<>
std::pair<std::string, bool> qvariantToStandardDataType<std::string>(const QVariant& data) {
  // conversion to string is allways possible, but we have to call toStdString
  // on QString
  return {data.value<QString>().toStdString(), true};
}

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const {
  return QString(_accessor[channelIndex][elementIndex].c_str());
}

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::dataAsHex(unsigned int channelIndex,
    unsigned int elementIndex) const {
  // The hex object is an integer representation. It generally does not make
  // sense for string data to return this object. We could throw, which would
  // have to be caught. It is simpler to return an empty variant.
  return QVariant();
}

template<>
QVariant RegisterTypeAbstractorImpl<int64_t>::data(unsigned int channelIndex, unsigned int elementIndex) const {
  return qlonglong(_accessor[channelIndex][elementIndex]);
}
template<>
QVariant RegisterTypeAbstractorImpl<uint64_t>::data(unsigned int channelIndex, unsigned int elementIndex) const {
  return qulonglong(_accessor[channelIndex][elementIndex]);
}
