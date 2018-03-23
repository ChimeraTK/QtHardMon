#include "RegisterTypeAbstractorImpl.h"

template <class USER_DATA_TYPE>
std::shared_ptr<RegisterTypeAbstractor> createTypedAccessor(ChimeraTK::RegisterInfo const & registerInfo, ChimeraTK::Device & device){
  ///@todo FIXME Create raw or coocked accessor
  auto accessor = device.getTwoDRegisterAccessor<USER_DATA_TYPE>(registerInfo.getRegisterName());
  return std::make_shared< RegisterTypeAbstractorImpl<USER_DATA_TYPE> >(accessor, registerInfo.getDataDescriptor().rawDataType()); 
}
                                                               
std::shared_ptr<RegisterTypeAbstractor> createAbstractAccessor(ChimeraTK::RegisterInfo const & registerInfo, ChimeraTK::Device & device){
  auto dataDescriptor = registerInfo.getDataDescriptor();
  switch(dataDescriptor.fundamentalType()){
  case ChimeraTK::RegisterInfo::FundamentalType::numeric:
    if (dataDescriptor.isIntegral()){
      if (dataDescriptor.isSigned()){
        return createTypedAccessor<int32_t>(registerInfo, device);
      }else{
        return createTypedAccessor<uint32_t>(registerInfo, device);
      }
    }else{//not integral, use double
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
    //fall into default. Nothing to display
  case ChimeraTK::RegisterInfo::FundamentalType::undefined:
    // fall into default, just mentioned for completeness
  default:
    return nullptr;
  }

}
