#include "NumericAddressedMultiplexedAreaQTreeItem.h"
#include "Exceptions.h"
#include "NumericAddressedSequenceRegisterQTreeItem.h"
#include "RegisterTreeUtilities.h"

NumericAddressedMultiplexedAreaQTreeItem::
    NumericAddressedMultiplexedAreaQTreeItem(
        mtca4u::Device &device,
        boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
        const mtca4u::RegisterCatalogue &catalogue,
        mtca4u::RegisterCatalogue::const_iterator &firstSequenceItem,
        QTreeWidget *parent, PropertiesWidgetProvider &propertiesWidgetProvider)
    : DeviceElementQTreeItem(
          QString(RegisterTreeUtilities::getRegisterName(registerInfo).c_str()),
          static_cast<int>(DeviceElementDataType::MultiplexedAreaDataType),
          RegisterTreeUtilities::assignToModuleItem(registerInfo, parent,
                                                    propertiesWidgetProvider),
          propertiesWidgetProvider),
      twoDRegisterAccessor_(device.getTwoDRegisterAccessor<double>(
          getTrimmedRegisterName(registerInfo))) {
  unsigned int nOfChannels = twoDRegisterAccessor_.getNChannels();

  for (unsigned int i = 0;
       i < nOfChannels && firstSequenceItem != catalogue.end();
       ++i, ++firstSequenceItem) {
    boost::shared_ptr<mtca4u::RegisterInfo> currentSequenceItem =
        catalogue.getRegister(firstSequenceItem->getRegisterName());
    DeviceElementQTreeItem *sequenceItem __attribute__((unused)) =
        new NumericAddressedSequenceRegisterQTreeItem(
            currentSequenceItem, twoDRegisterAccessor_, i, this,
            propertiesWidgetProvider);
  }

  name_ = registerInfo->getRegisterName().getComponents();

  mtca4u::RegisterInfoMap::RegisterInfo *numericAddressedRegisterInfo =
      dynamic_cast<mtca4u::RegisterInfoMap::RegisterInfo *>(registerInfo.get());

  if (numericAddressedRegisterInfo) {
    bar_ = numericAddressedRegisterInfo->bar;
    address_ = numericAddressedRegisterInfo->address;
    size_ = numericAddressedRegisterInfo->nBytes;
  } else {
    bar_ = -1;
    address_ = -1;
    size_ = -1;
    // FIXME: the cast was invalid, we have assigned wrong
    // DeviceElementQTreeItem.
  }

  --firstSequenceItem;
}

void NumericAddressedMultiplexedAreaQTreeItem::readData() {
  throw InvalidOperationException(
      "You cannot read from a multiplexed area. Select a sequence register.");
}

void NumericAddressedMultiplexedAreaQTreeItem::writeData() {
  throw InvalidOperationException(
      "You cannot write to a multiplexed area. Select a sequence register.");
}

std::string NumericAddressedMultiplexedAreaQTreeItem::getTrimmedRegisterName(
    boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  std::string regName = registerInfo->getRegisterName().getComponents().back();

  std::size_t prefixFound = regName.find("AREA_MULTIPLEXED_SEQUENCE_");
  if (prefixFound == 0) {
    return registerInfo->getRegisterName().getComponents().front() + "/" +
           regName.substr(26);
  } else {
    throw InternalErrorException(
        "Cannot determine proper multiplexed area and sequence descriptors.");
  }
}

void NumericAddressedMultiplexedAreaQTreeItem::updateRegisterProperties() {
  getPropertiesWidget()->clearFields();
  getPropertiesWidget()->setNames(name_);
  getPropertiesWidget()->setSize(twoDRegisterAccessor_.getNChannels(), size_);
  getPropertiesWidget()->setAddress(bar_, address_);
  RegisterPropertiesWidget *registerPropertiesCast =
      dynamic_cast<RegisterPropertiesWidget *>(getPropertiesWidget());
  if (registerPropertiesCast) {
    registerPropertiesCast->setFixedPointConverter(nullptr);
  }
}