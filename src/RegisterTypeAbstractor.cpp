#include "PreferencesProvider.h"
#include "RegisterTypeAbstractorRawImpl.h"

template<class USER_DATA_TYPE>
std::shared_ptr<RegisterTypeAbstractor> createTypedAccessor(ChimeraTK::RegisterInfo const& registerInfo,
    ChimeraTK::Device& device) {
  // Limit the number of elements in the accessor. Add 1 to be able to diplay
  // accessors with maxwords elements, and use the last element to show
  // truncated indicator in the model.
  auto nElements = std::min(PreferencesProviderSingleton::Instance().getValue<unsigned int>("maxWords") + 1,
      registerInfo.getNumberOfElements());

  auto dataDescriptor = registerInfo.getDataDescriptor();
  switch(dataDescriptor.rawDataType()) {
    case ChimeraTK::DataType::none: {
      ChimeraTK::TwoDRegisterAccessor<USER_DATA_TYPE> accessor;
      // if there is an unknown raw data type create a normal, cooked accessor

      if (not registerInfo.isWriteable()) {
        try {
          accessor.replace(device.getTwoDRegisterAccessor<USER_DATA_TYPE>(registerInfo.getRegisterName() / ".DUMMY_WRITEABLE", nElements));
        } catch (...) { }
      }

      if (not accessor.isInitialised())
          accessor.replace(device.getTwoDRegisterAccessor<USER_DATA_TYPE>(registerInfo.getRegisterName(), nElements));

      return std::make_shared<RegisterTypeAbstractorImpl<USER_DATA_TYPE>>(accessor, dataDescriptor.rawDataType());
    }
    default: {
      std::shared_ptr<RegisterTypeAbstractor> returnValue;
      auto rawAccessorCreatorLambda = [&](auto arg) {
        ChimeraTK::TwoDRegisterAccessor<decltype(arg)> accessor;
        if (not registerInfo.isWriteable()) {
          try {
            accessor.replace(device.getTwoDRegisterAccessor<decltype (arg)>(registerInfo.getRegisterName() / ".DUMMY_WRITEABLE", nElements, 0, {ChimeraTK::AccessMode::raw}));
          } catch (...) { }
        }

        if (not accessor.isInitialised()) {
          accessor.replace(device.getTwoDRegisterAccessor<decltype (arg)>(registerInfo.getRegisterName(), nElements, 0, {ChimeraTK::AccessMode::raw}));
        }
        returnValue = std::make_shared<RegisterTypeAbstractorRawImpl<decltype(arg), USER_DATA_TYPE>>(
            accessor, dataDescriptor.rawDataType());
      };
      callForTypeNoVoid(dataDescriptor.rawDataType(), rawAccessorCreatorLambda);
      return returnValue;
    }
  }
}

std::shared_ptr<RegisterTypeAbstractor> createAbstractAccessor(ChimeraTK::RegisterInfo const& registerInfo,
    ChimeraTK::Device& device) {
  auto dataDescriptor = registerInfo.getDataDescriptor();
  switch(dataDescriptor.fundamentalType()) {
    case ChimeraTK::RegisterInfo::FundamentalType::numeric:
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
    case ChimeraTK::RegisterInfo::FundamentalType::string:
      return createTypedAccessor<std::string>(registerInfo, device);
      break;
    case ChimeraTK::RegisterInfo::FundamentalType::boolean:
      return createTypedAccessor<uint32_t>(registerInfo, device);
      break;
    case ChimeraTK::RegisterInfo::FundamentalType::nodata:
      // fall into default. Nothing to display
    case ChimeraTK::RegisterInfo::FundamentalType::undefined:
      // fall into default, just mentioned for completeness
    default:
      return nullptr;
  }
}
