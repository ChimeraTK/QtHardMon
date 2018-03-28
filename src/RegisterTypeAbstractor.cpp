#include "RegisterTypeAbstractorRawImpl.h"

template <class USER_DATA_TYPE>
std::shared_ptr<RegisterTypeAbstractor> createTypedAccessor(ChimeraTK::RegisterInfo const & registerInfo, ChimeraTK::Device & device){
  ///@todo FIXME Use the new stuff from DeviceAccess to loop the whole list of supported data types.
  auto dataDescriptor = registerInfo.getDataDescriptor();
  switch(dataDescriptor.rawDataType()){
    case ChimeraTK::DataType::int32:{
      auto accessor = device.getTwoDRegisterAccessor<int32_t>(registerInfo.getRegisterName(),0,0,{ChimeraTK::AccessMode::raw});
      return std::make_shared< RegisterTypeAbstractorRawImpl<int32_t, USER_DATA_TYPE> >(accessor, dataDescriptor.rawDataType());
    }
    default:{
      // if there is an unknown raw data type create a normal, coocked accessor
      auto accessor = device.getTwoDRegisterAccessor<USER_DATA_TYPE>(registerInfo.getRegisterName());
      return std::make_shared<RegisterTypeAbstractorImpl<USER_DATA_TYPE> >(accessor, dataDescriptor.rawDataType());
    }
  }
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
