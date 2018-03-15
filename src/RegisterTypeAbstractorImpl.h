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
  bool setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data) override;

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

template<class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data){

  if (data.type() == QVariant::UserType){ //in the user type the data is hex representation
    if (! data.canConvert<HexData>() ){
      return false;
    }
    // The variant has a templated function called value() which returns the template type,
    // HexData in out case. The HexData class itself has a member called value, which holds the
    // actual value. Phew...
    _accessor[channelIndex][elementIndex] = data.value<HexData>().value;
  }else{
    if (! data.canConvert<USER_DATA_TYPE>() ){
      return false;
    }
    _accessor[channelIndex][elementIndex] = data.value<USER_DATA_TYPE>();
  }
  return true;
}

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
bool RegisterTypeAbstractorImpl<std::string>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data);

#endif // REGISTER_TYPE_ABSTRACTOR_IMPL_H
