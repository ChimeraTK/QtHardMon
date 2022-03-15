#include "PreferencesProvider.h"
#include "RegisterTypeAbstractorRawImpl.h"

template<class USER_DATA_TYPE>
std::shared_ptr<RegisterTypeAbstractor> createTypedAccessor(
    ChimeraTK::RegisterInfo const& registerInfo, ChimeraTK::Device& device) {
  // Limit the number of elements in the accessor. Add 1 to be able to diplay
  // accessors with maxwords elements, and use the last element to show
  // truncated indicator in the model.
  auto nElements = std::min(PreferencesProviderSingleton::Instance().getValue<unsigned int>("maxWords") + 1,
      registerInfo.getNumberOfElements());

  auto dataDescriptor = registerInfo.getDataDescriptor();
  if((dataDescriptor.rawDataType() == ChimeraTK::DataType::none) ||
      registerInfo.getSupportedAccessModes().has(ChimeraTK::AccessMode::wait_for_new_data)) {
    // We cannot request an accessor which has  raw and wait_for_new_data ath the same time.
    // If the register supports wait_for_new_data and don't use raw to be able to debug interrupt behaviour

    ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> accessor;
    // if there is an unknown raw data type create a normal, cooked accessor

    ChimeraTK::AccessModeFlags flags; // empty set of flags
    if(registerInfo.getSupportedAccessModes().has(ChimeraTK::AccessMode::wait_for_new_data)) {
      flags.add(ChimeraTK::AccessMode::wait_for_new_data);
    }
    if(not registerInfo.isWriteable()) {
      try {
        accessor.replace(device.getTwoDRegisterAccessor<USER_DATA_TYPE>(
            registerInfo.getRegisterName() / ".DUMMY_WRITEABLE", nElements, 0, flags));
      }
      catch(...) {
      }
    }

    if(not accessor.isInitialised())
      accessor.replace(
          device.getTwoDRegisterAccessor<USER_DATA_TYPE>(registerInfo.getRegisterName(), nElements, 0, flags));
    // Always use raw data type "none". For this cooked accessors we can't use it anyway (event if it's not none in
    // the catalogue if wait_for_new data is there)
    return std::make_shared<RegisterTypeAbstractorImpl<USER_DATA_TYPE>>(accessor, ChimeraTK::DataType::none);
  }
  else { // Accessor has a valid raw type and no wait_for_new_data. Use a raw accessor.
    std::shared_ptr<RegisterTypeAbstractor> returnValue;
    auto rawAccessorCreatorLambda = [&](auto arg) {
      ChimeraTK::TwoDRegisterAccessor<decltype(arg)> accessor;
      // NumeicAddressedBackend cannot create accessors with both raw and wait_for_new_data.
      // As the update rate in the GUI is limited and the update time is the determined by the timeout,
      // the reading is not being synchronous anyway, we go for raw data as this gives a visible benefit for the user.
      if(not registerInfo.isWriteable()) {
        try {
          accessor.replace(device.getTwoDRegisterAccessor<decltype(arg)>(
              registerInfo.getRegisterName() / ".DUMMY_WRITEABLE", nElements, 0, {ChimeraTK::AccessMode::raw}));
        }
        catch(...) {
        }
      }

      if(not accessor.isInitialised()) {
        accessor.replace(device.getTwoDRegisterAccessor<decltype(arg)>(
            registerInfo.getRegisterName(), nElements, 0, {ChimeraTK::AccessMode::raw}));
      }
      returnValue = std::make_shared<RegisterTypeAbstractorRawImpl<decltype(arg), USER_DATA_TYPE>>(
          accessor, dataDescriptor.rawDataType());
    };
    callForTypeNoVoid(dataDescriptor.rawDataType(), rawAccessorCreatorLambda);
    return returnValue;
  }
}

std::shared_ptr<RegisterTypeAbstractor> createAbstractAccessor(
    ChimeraTK::RegisterInfo const& registerInfo, ChimeraTK::Device& device) {
  auto dataDescriptor = registerInfo.getDataDescriptor();
  switch(dataDescriptor.fundamentalType()) {
    case ChimeraTK::DataDescriptor::FundamentalType::numeric:
      if(dataDescriptor.isIntegral()) {
        if(dataDescriptor.isSigned()) {
          return createTypedAccessor<int32_t>(registerInfo, device);
        }
        else {
          return createTypedAccessor<uint32_t>(registerInfo, device);
        }
      }
      else { // not integral, use double
        return createTypedAccessor<double>(registerInfo, device);
      }
      break;
    case ChimeraTK::DataDescriptor::FundamentalType::string:
      return createTypedAccessor<std::string>(registerInfo, device);
      break;
    case ChimeraTK::DataDescriptor::FundamentalType::boolean:
      return createTypedAccessor<uint32_t>(registerInfo, device);
      break;
    case ChimeraTK::DataDescriptor::FundamentalType::nodata:
      // fall into default. Nothing to display
    case ChimeraTK::DataDescriptor::FundamentalType::undefined:
      // fall into default, just mentioned for completeness
    default:
      return nullptr;
  }
}
