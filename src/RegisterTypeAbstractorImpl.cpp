#include "RegisterTypeAbstractorImpl.h"

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const{
  return QString(_accessor[channelIndex][elementIndex].c_str());
}
