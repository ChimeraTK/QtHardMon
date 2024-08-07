#include "PreferencesProvider.h"
#include "RegisterTypeAbstractorRawImpl.h"

#include <QDebug>

template<class USER_DATA_TYPE>
std::pair<std::shared_ptr<RegisterTypeAbstractor>, std::shared_ptr<RegisterTypeAbstractor>> createTypedAccessors(
    ChimeraTK::RegisterInfo const& registerInfo, ChimeraTK::Device& device) {
  // Limit the number of elements in the accessor. Add 1 to be able to diplay
  // accessors with maxwords elements, and use the last element to show
  // truncated indicator in the model.
  auto nElements = std::min(PreferencesProviderSingleton::Instance().getValue<unsigned int>("maxWords") + 1,
      registerInfo.getNumberOfElements());

  auto dataDescriptor = registerInfo.getDataDescriptor();
  if((dataDescriptor.rawDataType() == ChimeraTK::DataType::none) ||
      (dataDescriptor.rawDataType() == ChimeraTK::DataType::Void) ||
      registerInfo.getSupportedAccessModes().has(ChimeraTK::AccessMode::wait_for_new_data)) {
    // We cannot request an accessor which has  raw and wait_for_new_data ath the same time.
    // If the register supports wait_for_new_data and don't use raw to be able to debug interrupt behaviour
    // Also for data type void there is no sense for a raw accessor. User type int should have
    // been requested (but we don't check that here).

    ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> accessor;
    ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> dummyWritableAccessor;
    // if there is an unknown raw data type create a normal, cooked accessor

    if(not registerInfo.isWriteable()) {
      try {
        dummyWritableAccessor.replace(device.getTwoDRegisterAccessor<USER_DATA_TYPE>(
            registerInfo.getRegisterName() / ".DUMMY_WRITEABLE", nElements, 0, {}));
      }
      catch(...) {
      }
    }
    ChimeraTK::AccessModeFlags flags; // empty set of flags
    if(registerInfo.getSupportedAccessModes().has(ChimeraTK::AccessMode::wait_for_new_data)) {
      flags.add(ChimeraTK::AccessMode::wait_for_new_data);
    }
    accessor.replace(
        device.getTwoDRegisterAccessor<USER_DATA_TYPE>(registerInfo.getRegisterName(), nElements, 0, flags));
    // Always use raw data type "none". For this cooked accessors we can't use it anyway (event if it's not none in
    // the catalogue if wait_for_new data is there)
    auto abstractorWithTypeNone =
        std::make_shared<RegisterTypeAbstractorImpl<USER_DATA_TYPE>>(accessor, ChimeraTK::DataType::none);

    std::shared_ptr<RegisterTypeAbstractor> dummyAbstractorWithTypeNone;
    if(dummyWritableAccessor.isInitialised()) {
      dummyAbstractorWithTypeNone = std::make_shared<RegisterTypeAbstractorImpl<USER_DATA_TYPE>>(
          dummyWritableAccessor, ChimeraTK::DataType::none);
    }
    return std::pair(abstractorWithTypeNone, dummyAbstractorWithTypeNone);
  }

  // Accessor has a valid raw type and no wait_for_new_data. Use a raw accessor.
  std::shared_ptr<RegisterTypeAbstractor> returnValue;
  std::shared_ptr<RegisterTypeAbstractor> dummyReturnValue;
  auto rawAccessorCreatorLambda = [&](auto arg) {
    ChimeraTK::TwoDRegisterAccessor<decltype(arg)> accessor;
    ChimeraTK::TwoDRegisterAccessor<decltype(arg)> dummyWritableAccessor;
    // NumeicAddressedBackend cannot create accessors with both raw and wait_for_new_data.
    // As the update rate in the GUI is limited and the update time is the determined by the timeout,
    // the reading is not being synchronous anyway, we go for raw data as this gives a visible benefit for the user.
    if(not registerInfo.isWriteable()) {
      try {
        dummyWritableAccessor.replace(device.getTwoDRegisterAccessor<decltype(arg)>(
            registerInfo.getRegisterName() / ".DUMMY_WRITEABLE", nElements, 0, {ChimeraTK::AccessMode::raw}));
      }
      catch(...) {
      }
    }

    accessor.replace(device.getTwoDRegisterAccessor<decltype(arg)>(
        registerInfo.getRegisterName(), nElements, 0, {ChimeraTK::AccessMode::raw}));
    returnValue = std::make_shared<RegisterTypeAbstractorRawImpl<decltype(arg), USER_DATA_TYPE>>(
        accessor, dataDescriptor.rawDataType());
    if(dummyWritableAccessor.isInitialised()) {
      dummyReturnValue = std::make_shared<RegisterTypeAbstractorRawImpl<decltype(arg), USER_DATA_TYPE>>(
          dummyWritableAccessor, dataDescriptor.rawDataType());
    }
  };
  callForTypeNoVoid(dataDescriptor.rawDataType(), rawAccessorCreatorLambda);
  return std::pair(returnValue, dummyReturnValue);
}

std::pair<std::shared_ptr<RegisterTypeAbstractor>, std::shared_ptr<RegisterTypeAbstractor>> createAbstractAccessors(
    ChimeraTK::RegisterInfo const& registerInfo, ChimeraTK::Device& device) {
  auto dataDescriptor = registerInfo.getDataDescriptor();
  switch(dataDescriptor.fundamentalType()) {
    case ChimeraTK::DataDescriptor::FundamentalType::numeric:
      if(dataDescriptor.isIntegral()) {
        if(dataDescriptor.isSigned()) {
          return createTypedAccessors<int32_t>(registerInfo, device);
        }
        return createTypedAccessors<uint32_t>(registerInfo, device);
      }
      else { // not integral, use double
        return createTypedAccessors<double>(registerInfo, device);
      }
      break;
    case ChimeraTK::DataDescriptor::FundamentalType::string:
      return createTypedAccessors<std::string>(registerInfo, device);
      break;
    case ChimeraTK::DataDescriptor::FundamentalType::boolean:
      return createTypedAccessors<uint32_t>(registerInfo, device);
      break;
    case ChimeraTK::DataDescriptor::FundamentalType::nodata:
      // The accessor might be writeable, and even if there is not data the update time is displayed.
      // Create an int accessor (it's always a 2d accessor which cannot be void).
      return createTypedAccessors<int32_t>(registerInfo, device);
      break;
    case ChimeraTK::DataDescriptor::FundamentalType::undefined:
      // fall into default, just mentioned for completeness
    default:
      return std::make_pair(nullptr, nullptr);
      ;
  }
}
