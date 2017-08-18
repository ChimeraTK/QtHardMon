#include "NumericAddressedCookedMultiplexedAreaQTreeItem.h"
#include "Exceptions.h"
#include "ModuleQTreeItem.h"
#include <iterator>
#include "NumericAddressedCookedSequenceRegisterQTreeItem.h"
#include "RegisterTreeUtilities.h"

NumericAddressedCookedMultiplexedAreaQTreeItem::NumericAddressedCookedMultiplexedAreaQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, PropertiesWidgetProvider & propertiesWidgetProvider)
: DeviceElementQTreeItem(QString(RegisterTreeUtilities::getRegisterName(registerInfo).c_str()),
  static_cast<int>(DeviceElementDataType::MultiplexedAreaDataType), 
  RegisterTreeUtilities::assignToModuleItem(registerInfo, parent, propertiesWidgetProvider), propertiesWidgetProvider),
  twoDRegisterAccessor_(device.getTwoDRegisterAccessor<double>(registerInfo->getRegisterName()))
  {
    unsigned int nOfChannels = twoDRegisterAccessor_.getNChannels();

    for (int i = 0; i < nOfChannels; ++i) {
        DeviceElementQTreeItem * sequenceItem = new NumericAddressedCookedSequenceRegisterQTreeItem(registerInfo, twoDRegisterAccessor_, i, this, propertiesWidgetProvider);
    }

    name_ = registerInfo->getRegisterName().getComponents();

    mtca4u::RegisterInfoMap::RegisterInfo * numericAddressedRegisterInfo = dynamic_cast<mtca4u::RegisterInfoMap::RegisterInfo *>(registerInfo.get());

    if (numericAddressedRegisterInfo) {
        bar_ = numericAddressedRegisterInfo->bar;
        address_ = numericAddressedRegisterInfo->address;
        size_ = numericAddressedRegisterInfo->nBytes;
    } else {
        bar_ = -1;
        address_ = -1;
        size_ = -1;
        // FIXME: the cast was invalid, we have assigned wrong DeviceElementQTreeItem.
    }
}

void NumericAddressedCookedMultiplexedAreaQTreeItem::readData() {
throw InvalidOperationException(
      "You cannot read from a multiplexed area. Select a sequence register.");
}

void NumericAddressedCookedMultiplexedAreaQTreeItem::writeData() {
  throw InvalidOperationException(
      "You cannot write to a multiplexed area. Select a sequence register.");
}

void NumericAddressedCookedMultiplexedAreaQTreeItem::updateRegisterProperties() {
    getPropertiesWidget()->clearFields();
    getPropertiesWidget()->setNames(name_);
    getPropertiesWidget()->setSize(twoDRegisterAccessor_.getNChannels(), size_);
    getPropertiesWidget()->setAddress(bar_, address_);
}