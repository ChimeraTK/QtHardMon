#ifndef REGISTER_TYPE_ABSTRACTOR_RAW_IMPL_H
#define REGISTER_TYPE_ABSTRACTOR_RAW_IMPL_H

#include "RegisterTypeAbstractorImpl.h"

/** This is a double templated accessor abstractor with raw and cooked data
 * type. It is based upon the single-typed RegisterTypeAbstractorImpl with the
 * raw type as user type. For those functions which do cooked values, it does
 * the corresponding conversion.
 */
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
class RegisterTypeAbstractorRawImpl : public RegisterTypeAbstractorImpl<RAW_DATA_TYPE> {
 public:
  RegisterTypeAbstractorRawImpl(
      ChimeraTK::TwoDRegisterAccessor<RAW_DATA_TYPE> const& accessor, ChimeraTK::DataType const& rawDataType);
  using RegisterTypeAbstractorImpl<RAW_DATA_TYPE>::_accessor;

  /// Override because it returns cooked data while the accessor under the hood
  /// is raw.
  QVariant data(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because it returns cooked data while the accessor under the hood
  /// is raw.
  QVariant dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because it expects cooked data while the accessor under the hood
  /// is raw.
  bool setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) override;

  /// Override because not implemented in the single types base implementation
  /// without raw access
  QVariant rawData(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because not implemented in the single types base implementation
  /// without raw access
  QVariant rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  /// Override because not implemented in the single types base implementation
  /// without raw access
  bool setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) override;
  /// Override to update the cached cooked data after reading
  void read(bool allowBlockingRead) override;

  /// Override because this is for the cooked type while the base implementation
  /// would return for the raw type.
  bool isIntegral() const override;

  void interrupt() override;

 protected:
  /// Cached data for painting. If exceptions occur during data conversion this
  /// does not happen every time when painting.
  std::vector<std::vector<COOKED_DATA_TYPE>> _cachedCookedData;
  bool _cachedCookedDataValid;

  void updateCachedCookedData();
};

/// Helper function to abstract that we need special code for QString, which
/// cannot be created from std::string. We can easily do a template
/// specialisation for this little function, while a partial specialisation of
/// the two parameter template class RegisterTypeAbstractorRawImpl would be way
/// more complicated.
template<typename DATA_TYPE>
QVariant dataToQVariant(DATA_TYPE data) {
  return QVariant(data);
}

template<>
QVariant dataToQVariant<std::string>(std::string data);

template<>
QVariant dataToQVariant<int64_t>(int64_t);

template<>
QVariant dataToQVariant<uint64_t>(uint64_t);

template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::RegisterTypeAbstractorRawImpl(
    ChimeraTK::TwoDRegisterAccessor<RAW_DATA_TYPE> const& accessor, ChimeraTK::DataType const& rawDataType)
: RegisterTypeAbstractorImpl<RAW_DATA_TYPE>(accessor, rawDataType), _cachedCookedDataValid(false) {
  _cachedCookedData.resize(_accessor.getNChannels());
  for(auto& channelCookedData : _cachedCookedData) {
    channelCookedData.resize(_accessor.getNElementsPerChannel());
  }
  updateCachedCookedData();
}

// get the data as cooked and return a QVariant
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::data(
    unsigned int channelIndex, unsigned int elementIndex) const {
  if(_cachedCookedDataValid) {
    return dataToQVariant(_cachedCookedData[channelIndex][elementIndex]);
  }
  else {
    return QVariant();
  }
}

// Get the data as cooked and put it into the HexData user type. Return this as
// QVariant. Notice: For QString cooked dataq the correct return value would be
// an empty variant, which would require a partial template specialisation.
// Instead, the HexData is accepting a string in the constructor and has value
// 0. As this function instantiation should never be called, it should not
// matter.
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::dataAsHex(
    unsigned int channelIndex, unsigned int elementIndex) const {
  QVariant returnValue;
  if(_cachedCookedDataValid) {
    returnValue.setValue(HexData(_cachedCookedData[channelIndex][elementIndex]));
  }
  return returnValue;
}

// Directly use the accessor, which is a raw accessor, and return the value as
// QVariant
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::rawData(
    unsigned int channelIndex, unsigned int elementIndex) const {
  return dataToQVariant(_accessor[channelIndex][elementIndex]);
}

// Directly use the accessor, which is a raw accessor, put the  value into
// HexData and return as QVariant
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
QVariant RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::rawDataAsHex(
    unsigned int channelIndex, unsigned int elementIndex) const {
  QVariant returnValue;
  returnValue.setValue(HexData(_accessor[channelIndex][elementIndex]));
  return returnValue;
}

// read and then update the cached cooked data
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
void RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::read(bool allowBlockingRead) {
  if(!_accessor.isReadable()) return;
  // Implementation for wait_for_new_data:
  // - call readLatest() to get the last received value. If nothing has been received yet call a blocking read.
  // Also works without wait_for_new_data because readLatest() always returns true in this case.
  if(!_accessor.readLatest() && allowBlockingRead) {
    _accessor.read();
  }
  updateCachedCookedData();
}

// Return whether the cooked data type is integral
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
bool RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::isIntegral() const {
  return std::is_integral<COOKED_DATA_TYPE>::value;
}

// set the data using setAsCooked
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
bool RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::setData(
    unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) {
  auto conversionResult = qvariantToStandardDataType<COOKED_DATA_TYPE>(data);
  if(conversionResult.second) { // conversion successful
    _accessor.template setAsCooked<COOKED_DATA_TYPE>(channelIndex, elementIndex, conversionResult.first);
    updateCachedCookedData();
    return true;
  }
  else {
    return false;
  }
}

// set the raw data, which means directly using the raw accessor
template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
bool RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::setRawData(
    unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) {
  auto conversionResult = qvariantToStandardDataType<RAW_DATA_TYPE>(data);
  if(conversionResult.second) {
    _accessor[channelIndex][elementIndex] = conversionResult.first;
    updateCachedCookedData();
    return true;
  }
  else {
    return false;
  }
}

template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
void RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::updateCachedCookedData() {
  _cachedCookedDataValid = false; // set to false in case something goes wrong during conversion
  for(size_t channelIndex = 0; channelIndex < _cachedCookedData.size(); ++channelIndex) {
    auto& cookedChan = _cachedCookedData[channelIndex];
    for(size_t elementIndex = 0; elementIndex < cookedChan.size(); ++elementIndex) {
      cookedChan[elementIndex] = _accessor.template getAsCooked<COOKED_DATA_TYPE>(channelIndex, elementIndex);
    }
  }
  _cachedCookedDataValid = true;
}

template<class RAW_DATA_TYPE, class COOKED_DATA_TYPE>
void RegisterTypeAbstractorRawImpl<RAW_DATA_TYPE, COOKED_DATA_TYPE>::interrupt() {
  if(_accessor.getAccessModeFlags().has(ChimeraTK::AccessMode::wait_for_new_data)) {
    _accessor.getHighLevelImplElement()->interrupt();
  }
}

#endif // REGISTER_TYPE_ABSTRACTOR_RAW_IMPL_H
