#ifndef REGISTER_TYPE_ABSTRACTOR_RAW_IMPL_H
#define REGISTER_TYPE_ABSTRACTOR_RAW_IMPL_H

#include "RegisterTypeAbstractorImpl.h"

/** This is a double templated accessor abstractor with raw and coocked data type.
 *  It is based upon the single-typed RegisterTypeAbstractorImpl with the raw type
 *  as user type. For those functions which do coocked values, it does the corresponding
 *  conversion.
 */
template <class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
class RegisterTypeAbstractorRawImpl: public RegisterTypeAbstractorImpl<RAW_DATA_TYPE>{
 public:
  using RegisterTypeAbstractorImpl<RAW_DATA_TYPE>::RegisterTypeAbstractorImpl;
  using RegisterTypeAbstractorImpl<RAW_DATA_TYPE>::_accessor;

  /// Override because it returns coocked data while the accessor under the hood is raw.
  QVariant data(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because it returns coocked data while the accessor under the hood is raw.
  QVariant dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because it expects coocked data while the accessor under the hood is raw.
  bool setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data) override;

  /// Override because not implemented in the single types base implementation without raw access
  QVariant rawData(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because not implemented in the single types base implementation without raw access
  QVariant rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because not implemented in the single types base implementation without raw access
  bool setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data) override;

  /// Override because this is for the coocked type while the base implementation would return
  /// for the raw type.
  bool isIntegral() const override;
};

/// Helper function to abstract that we need special code for QString, which cannot be created
/// from std::string. We can easily do a template specialisation for this little function, while
/// a partial specialisation of the two parameter template class RegisterTypeAbstractorRawImpl
/// would be way more complicated.
template<typename DATA_TYPE>
QVariant dataToQVariant(DATA_TYPE data){
  return QVariant(data);
}

template<>
QVariant dataToQVariant<std::string>(std::string data);

// get the data as coocked and return a QVariant
template <class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOCKED_DATA_TYPE>::data(unsigned int channelIndex, unsigned int elementIndex) const{
  return dataToQVariant(_accessor.template getAsCoocked<COOCKED_DATA_TYPE>(channelIndex,elementIndex));
}

// Get the data as coocked and put it into the HexData user type. Return this as QVariant.
// Notice: For QString coocked dataq the correct return value would be an empty variant, which
// would require a partial template specialisation. Instead, the HexData is accepting
// a string in the constructor and has value 0. As this function instantiation should never be
// called, it should not matter.
template <class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOCKED_DATA_TYPE>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const{
  QVariant returnValue;
  returnValue.setValue(HexData(_accessor.template getAsCoocked<COOCKED_DATA_TYPE>(channelIndex,elementIndex)));
  return returnValue;
}

// Directly use the accessor, which is a raw accessor, and return the value as QVariant
template <class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOCKED_DATA_TYPE>::rawData(unsigned int channelIndex, unsigned int elementIndex) const{
  return dataToQVariant(_accessor[channelIndex][elementIndex]);
}

// Directly use the accessor, which is a raw accessor, put the  value into HexData and return as QVariant
template <class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOCKED_DATA_TYPE>::rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const{
  QVariant returnValue;
  returnValue.setValue(HexData(_accessor[channelIndex][elementIndex]));
  return returnValue;
}

// Return whether the coocked data type is integral
template <class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
bool RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOCKED_DATA_TYPE>::isIntegral() const{
  return std::is_integral<COOCKED_DATA_TYPE>::value;
}

// set the data using setAsCoocked
template<class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
bool RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOCKED_DATA_TYPE>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data){

  auto conversionResult = qvariantToStandardDataType<COOCKED_DATA_TYPE>(data);
  if (conversionResult.second) {// conversion successful
    _accessor[channelIndex][elementIndex].template setAsCoocked<COOCKED_DATA_TYPE>(conversionResult.first);
    return true;
  }else{
    return false;
  }
}

// set the raw data, which means directly using the raw accessor
template<class RAW_DATA_TYPE, class COOCKED_DATA_TYPE>
bool RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOCKED_DATA_TYPE>::setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & data){

  auto conversionResult = qvariantToStandardDataType<RAW_DATA_TYPE>(data);
  if (conversionResult.second){
    _accessor[channelIndex][elementIndex] = conversionResult.first;
    return true;
  }else{
    return false;
  }
}

#endif // REGISTER_TYPE_ABSTRACTOR_RAW_IMPL_H
