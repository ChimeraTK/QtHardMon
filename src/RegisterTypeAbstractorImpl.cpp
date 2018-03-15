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
bool RegisterTypeAbstractorImpl<std::string>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value){
  _accessor[channelIndex][elementIndex] = value.toString().toStdString();
  return true;
}

template<>
bool RegisterTypeAbstractorImpl<double>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value){
  bool isConversionSuccessful=false;
  double convertedValue = value.toDouble(&isConversionSuccessful);
  // only write if conversion is successful. Value is 0.0 otherwise
  if (isConversionSuccessful){
    _accessor[channelIndex][elementIndex] = convertedValue;
  }
  
  return isConversionSuccessful;
}

template<>
bool RegisterTypeAbstractorImpl<int32_t>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value){
  bool isConversionSuccessful=false;
  int32_t convertedValue;
  std::cout << "This is setData with data type " << value.type() << std::endl;
  if (value.type() == QVariant::UserType){ //in the user type the data is hex representation, so
    isConversionSuccessful = value.canConvert<HexData>();
    std::cout << "value is " << value.value<HexData>().value << std::endl;
    // we have to tell this to the converter
    convertedValue = value.value<HexData>().value;
  }else{
    convertedValue = value.toInt(&isConversionSuccessful);
  }
  // only write if conversion is successful. Value is 0.0 otherwise
  if (isConversionSuccessful){
    _accessor[channelIndex][elementIndex] = convertedValue;
  }
  
  return isConversionSuccessful;
}

template<>
bool RegisterTypeAbstractorImpl<uint32_t>::setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant & value){
  bool isConversionSuccessful=false;
  uint32_t convertedValue;
  std::cout << "This is setData with data type " << value.type() << std::endl;
  if (value.type() == QVariant::UserType){ //in the user type the data is hex representation, so
    isConversionSuccessful = value.canConvert<HexData>();
    std::cout << "value is " << value.value<HexData>().value << std::endl;
    // we have to tell this to the converter
    convertedValue = value.value<HexData>().value;
  }else{
    convertedValue = value.toUInt(&isConversionSuccessful);
  }
  // only write if conversion is successful. Value is 0.0 otherwise
  if (isConversionSuccessful){
    _accessor[channelIndex][elementIndex] = convertedValue;
  }
  
  return isConversionSuccessful;
}
