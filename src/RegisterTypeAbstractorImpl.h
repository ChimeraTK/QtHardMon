#ifndef REGISTER_TYPE_ABSTRACTOR_IMPL_H
#define REGISTER_TYPE_ABSTRACTOR_IMPL_H

#include "RegisterTypeAbstractor.h"
#include <ChimeraTK/Device.h>

///The actual, templated implementation of the type abstractor.
template <class USER_DATA_TYPE>
class RegisterTypeAbstractorImpl: public RegisterTypeAbstractor{
 public:
  RegisterTypeAbstractorImpl(ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> const & accessor);
  unsigned int nChannels() const override;
  unsigned int nElements() const override;
  QVariant data(unsigned int channelIndex, unsigned int elementIndex) const override;
  void read() override;
  
 protected:
  ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> _accessor;
};

template <class USER_DATA_TYPE>
RegisterTypeAbstractorImpl<USER_DATA_TYPE>::RegisterTypeAbstractorImpl(ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> const & accessor) : _accessor(accessor){
}

template <class USER_DATA_TYPE>
unsigned int RegisterTypeAbstractorImpl<USER_DATA_TYPE>::nChannels() const{
  return _accessor.getNChannels();
}

template <class USER_DATA_TYPE>
unsigned int RegisterTypeAbstractorImpl<USER_DATA_TYPE>::nElements() const{
  return _accessor.getNElementsPerChannel();
}

template <class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::data(unsigned int channelIndex, unsigned int elementIndex) const{
  return QString::number(_accessor[channelIndex][elementIndex]);
}

template <class USER_DATA_TYPE>
void RegisterTypeAbstractorImpl<USER_DATA_TYPE>::read(){
  _accessor.read();
}

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const;


#endif // REGISTER_TYPE_ABSTRACTOR_IMPL_H
