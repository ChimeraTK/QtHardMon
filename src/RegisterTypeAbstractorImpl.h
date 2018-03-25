#ifndef REGISTER_TYPE_ABSTRACTOR_IMPL_H
#define REGISTER_TYPE_ABSTRACTOR_IMPL_H

#include "RegisterTypeAbstractor.h"
#include <ChimeraTK/Device.h>
#include "HexSpinBox.h"

///The actual, templated implementation of the type abstractor.
template <class USER_DATA_TYPE>
class RegisterTypeAbstractorImpl: public RegisterTypeAbstractor{
 public:
  RegisterTypeAbstractorImpl(ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> const & accessor, ChimeraTK::DataType const & rawDataType);
  unsigned int nChannels() const override;
  unsigned int nElements() const override;
  QVariant data(unsigned int channelIndex, unsigned int elementIndex) const override;
  QVariant dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  bool setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data) override;
  QVariant rawData(unsigned int channelIndex, unsigned int elementIndex) const override;
  QVariant rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  bool setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data) override;

  bool isIntegral() const override;
  ChimeraTK::DataType rawDataType() const override;

  void read() override;
  void write() override;
  
 protected:
  ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> _accessor;
  ChimeraTK::DataType _rawDataType;
  USER_DATA_TYPE get(unsigned int channelIndex, unsigned int elementIndex) const;
  std::pair<USER_DATA_TYPE, bool> toUserType(const QVariant & data) const;
};

template <class USER_DATA_TYPE>
RegisterTypeAbstractorImpl<USER_DATA_TYPE>::RegisterTypeAbstractorImpl(ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> const & accessor, ChimeraTK::DataType const & rawDataType)
 : _accessor(accessor), _rawDataType(rawDataType){
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
USER_DATA_TYPE RegisterTypeAbstractorImpl<USER_DATA_TYPE>::get(unsigned int channelIndex, unsigned int elementIndex) const{
  if (_rawDataType==ChimeraTK::DataType::none){
    // This is not a raw accessor. Directly access the data
    return _accessor[channelIndex][elementIndex];
  }else{
    // A raw accessor must get the coocked data via the special function of the raw accessor
    return _accessor.template getAsCoocked<USER_DATA_TYPE>(channelIndex,elementIndex);
  }
}

template <class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::data(unsigned int channelIndex, unsigned int elementIndex) const{
  return QVariant(get(channelIndex,elementIndex));
}

template <class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const{
  QVariant returnValue;
  returnValue.setValue(HexData(get(channelIndex,elementIndex)));
  return returnValue;
}

template <class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::rawData(unsigned int channelIndex, unsigned int elementIndex) const{
  // Only to be called when it's a raw accessor. Otherwise you will get the coocked data.
  return QVariant(_accessor[channelIndex][elementIndex]);
}

template <class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const{
  QVariant returnValue;
  // Only to be called when it's a raw accessor. Otherwise you will get the coocked data.
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

template <class USER_DATA_TYPE>
ChimeraTK::DataType RegisterTypeAbstractorImpl<USER_DATA_TYPE>::rawDataType() const{
  return _rawDataType;
}

template<class USER_DATA_TYPE>
std::pair<USER_DATA_TYPE, bool> RegisterTypeAbstractorImpl<USER_DATA_TYPE>::toUserType(const QVariant & data) const{

  if (data.type() == QVariant::UserType){ //in the user type the data is hex representation
    if (! data.canConvert<HexData>() ){
      return {USER_DATA_TYPE(), false};
    }
    // The variant has a templated function called value() which returns the template type,
    // HexData in out case. The HexData class itself has a member called value, which holds the
    // actual value. Phew...
    return {data.value<HexData>().value, true};
  }else{
    if (! data.canConvert<USER_DATA_TYPE>() ){
      return {USER_DATA_TYPE(), false};
    }
    return {data.value<USER_DATA_TYPE>(), true};
  }
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
template<class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data){

  auto conversionResult = toUserType(data);
  if (conversionResult.second){
    _accessor[channelIndex][elementIndex] = conversionResult.first;
    return true;
  }else{
    return false;
  }
}

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
bool RegisterTypeAbstractorImpl<std::string>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data);

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::rawData(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
bool RegisterTypeAbstractorImpl<std::string>::setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data);

#endif // REGISTER_TYPE_ABSTRACTOR_IMPL_H
