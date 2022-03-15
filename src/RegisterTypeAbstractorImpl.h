#ifndef REGISTER_TYPE_ABSTRACTOR_IMPL_H
#define REGISTER_TYPE_ABSTRACTOR_IMPL_H

#include "HexSpinBox.h"
#include "RegisterTypeAbstractor.h"
#include <ChimeraTK/Device.h>
#include <utility> // for std::pair

/// The actual, templated implementation of the type abstractor.
template<class USER_DATA_TYPE>
class RegisterTypeAbstractorImpl : public RegisterTypeAbstractor {
 public:
  RegisterTypeAbstractorImpl(
      ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> const& accessor, ChimeraTK::DataType const& rawDataType);
  unsigned int nChannels() const override;
  unsigned int nElements() const override;
  QVariant data(unsigned int channelIndex, unsigned int elementIndex) const override;
  QVariant dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  bool setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) override;
  QVariant rawData(unsigned int channelIndex, unsigned int elementIndex) const override;
  QVariant rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const override;
  bool setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) override;

  bool isIntegral() const override;
  ChimeraTK::DataType rawDataType() const override;
  bool hasWaitForNewData() const override;

  bool readLatest() override;
  void write() override;

  bool isWritable() override { return _accessor.isWriteable(); }
  bool isReadable() override { return _accessor.isReadable(); }
  void interrupt() override;
  ChimeraTK::VersionNumber getVersionNumber() override { return _accessor.getVersionNumber(); }

 protected:
  ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> _accessor;
  ChimeraTK::DataType _rawDataType;
};

/** A function to convert a QVariant to a standard data type, with special
 * treatment for hex data. Needed in several places with varying data types (raw
 * or cooked), so it is implemented as a stand-alone template function.
 *
 *  The second argument is the flag reported by the variant whether the
 * conversion was successful. If this is not the case, the data type value is
 * DATA_TYPE() (usually 0 or empty string).
 */
template<class DATA_TYPE>
std::pair<DATA_TYPE, bool> qvariantToStandardDataType(const QVariant& data) {
  if(data.type() == HexDataType) {
    if(!data.canConvert<HexData>()) {
      return {DATA_TYPE(), false};
    }
    // The variant has a templated function called value() which returns the
    // template type, HexData in our case. The HexData class itself has a member
    // called value, which holds the actual value. Phew...
    return {data.value<HexData>().value, true};
  }
  else {
    if(!data.canConvert<DATA_TYPE>()) {
      return {DATA_TYPE(), false};
    }
    return {data.value<DATA_TYPE>(), true};
  }
}

/*****************************************************************************************************/
/********** The Implementatons
 * ***********************************************************************/
/*****************************************************************************************************/
template<class USER_DATA_TYPE>
RegisterTypeAbstractorImpl<USER_DATA_TYPE>::RegisterTypeAbstractorImpl(
    ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> const& accessor, ChimeraTK::DataType const& rawDataType)
: _accessor(accessor), _rawDataType(rawDataType) {}

template<class USER_DATA_TYPE>
unsigned int RegisterTypeAbstractorImpl<USER_DATA_TYPE>::nChannels() const {
  return _accessor.getNChannels();
}

template<class USER_DATA_TYPE>
unsigned int RegisterTypeAbstractorImpl<USER_DATA_TYPE>::nElements() const {
  return _accessor.getNElementsPerChannel();
}

template<class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::data(unsigned int channelIndex, unsigned int elementIndex) const {
  return QVariant(_accessor[channelIndex][elementIndex]);
}

template<class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::dataAsHex(
    unsigned int channelIndex, unsigned int elementIndex) const {
  QVariant returnValue;
  returnValue.setValue(HexData(_accessor[channelIndex][elementIndex]));
  return returnValue;
}

template<class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::readLatest() {
  return _accessor.readLatest();
}

template<class USER_DATA_TYPE>
void RegisterTypeAbstractorImpl<USER_DATA_TYPE>::write() {
  _accessor.write();
}

template<class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::isIntegral() const {
  return std::is_integral<USER_DATA_TYPE>::value;
}

template<class USER_DATA_TYPE>
ChimeraTK::DataType RegisterTypeAbstractorImpl<USER_DATA_TYPE>::rawDataType() const {
  return _rawDataType;
}

template<class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::hasWaitForNewData() const {
  return _accessor.getAccessModeFlags().has(ChimeraTK::AccessMode::wait_for_new_data);
}

template<class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::setData(
    unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) {
  auto conversionResult = qvariantToStandardDataType<USER_DATA_TYPE>(data);
  if(conversionResult.second) {
    _accessor[channelIndex][elementIndex] = conversionResult.first;
    return true;
  }
  else {
    return false;
  }
}

/*****************************************************************************************************/
/********** (Emtpty) Implementations for Raw Access
 * **************************************************/
/*****************************************************************************************************/

template<class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::rawData(
    unsigned int channelIndex, unsigned int elementIndex) const {
  std::ignore = channelIndex;
  std::ignore = elementIndex;

  // Not useful for this kind of abstractor without raw data. Just return an
  // empty variant.
  return QVariant();
}

template<class USER_DATA_TYPE>
QVariant RegisterTypeAbstractorImpl<USER_DATA_TYPE>::rawDataAsHex(
    unsigned int channelIndex, unsigned int elementIndex) const {
  std::ignore = channelIndex;
  std::ignore = elementIndex;

  // Not useful for this kind of abstractor without raw data. Just return an
  // empty variant.
  return QVariant();
}

template<class USER_DATA_TYPE>
bool RegisterTypeAbstractorImpl<USER_DATA_TYPE>::setRawData(
    unsigned int channelIndex, unsigned int elementIndex, const QVariant& data) {
  std::ignore = channelIndex;
  std::ignore = elementIndex;
  std::ignore = data;

  // Not useful for this kind of abstractor without raw data. Just return false.
  return false;
}
template<class USER_DATA_TYPE>
void RegisterTypeAbstractorImpl<USER_DATA_TYPE>::interrupt() {
  if(_accessor.getAccessModeFlags().has(ChimeraTK::AccessMode::wait_for_new_data)) {
    _accessor.getHighLevelImplElement()->interrupt();
  }
}

/*****************************************************************************************************/
/********** Template specialisations
 * *****************************************************************/
/*****************************************************************************************************/
template<>
std::pair<std::string, bool> qvariantToStandardDataType<std::string>(const QVariant& data);

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::data(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
QVariant RegisterTypeAbstractorImpl<std::string>::dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const;

template<>
QVariant RegisterTypeAbstractorImpl<int64_t>::data(unsigned int channelIndex, unsigned int elementIndex) const;
template<>
QVariant RegisterTypeAbstractorImpl<uint64_t>::data(unsigned int channelIndex, unsigned int elementIndex) const;

#endif // REGISTER_TYPE_ABSTRACTOR_IMPL_H
