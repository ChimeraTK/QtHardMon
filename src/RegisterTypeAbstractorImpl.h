#ifndef REGISTER_TYPE_ABSTRACTOR_IMPL_H
#define REGISTER_TYPE_ABSTRACTOR_IMPL_H

#include "RegisterTypeAbstractor.h"
#include <ChimeraTK/Device.h>
#include "HexSpinBox.h"

///The actual, templated implementation of the type abstractor.
template <class USER_DATA_TYPE>
class RegisterTypeAbstractorImpl: public RegisterTypeAbstractor{
 public:
  RegisterTypeAbstractorImpl(ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> const & accessor);
  unsigned int nChannels() const override;
  unsigned int nElements() const override;
  QVariant data(unsigned int channelIndex, unsigned int elementIndex) const override;
  QVariant dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  bool setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value) override;

  bool isIntegral() const override;
  
  void read() override;
  void write() override;
  
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
  return QVariant(_accessor[channelIndex][elementIndex]);
}

template <class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const{
  QVariant returnValue;
  returnValue.setValue(HexData(_accessor[channelIndex][elementIndex]));
  return returnValue;
}

template <class USER_DATA_TYPE>
void RegisterTypeAbstractorImpl<USER_DATA_TYPE>::read(){
  _accessor.read();
}

template <class USER_DATA_TYPE>
void RegisterTypeAbstractorImpl<USER_DATA_TYPE>::write(){
  _accessor.write();
}

template <class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::isIntegral() const{
  return std::is_integral<USER_DATA_TYPE>::value;
}

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const;

// unfortunately we have to write a specialisation for all used data types
template<>
bool RegisterTypeAbstractorImpl<std::string>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value);

template<>
bool RegisterTypeAbstractorImpl<double>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value);

template<>
bool RegisterTypeAbstractorImpl<int32_t>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value);

template<>
bool RegisterTypeAbstractorImpl<uint32_t>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value);


#endif // REGISTER_TYPE_ABSTRACTOR_IMPL_H
