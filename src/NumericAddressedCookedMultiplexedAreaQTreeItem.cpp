#include "NumericAddressedCookedMultiplexedAreaQTreeItem.h"
#include "Exceptions.h"
#include "ModuleQTreeItem.h"
#include <iterator>
#include "NumericAddressedCookedSequenceRegisterQTreeItem.h"
#include "RegisterTreeUtilities.h"

NumericAddressedCookedMultiplexedAreaQTreeItem::NumericAddressedCookedMultiplexedAreaQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget)
: DeviceElementQTreeItem(QString(RegisterTreeUtilities::getRegisterName(registerInfo).c_str()),
  static_cast<int>(DeviceElementDataType::NumAddressedRegisterDataType), 
  RegisterTreeUtilities::assignToModuleItem(registerInfo, parent, propertiesWidget)),
  twoDRegisterAccessor_(device.getTwoDRegisterAccessor<double>(registerInfo->getRegisterName())),
  propertiesWidget_(propertiesWidget)
{
    unsigned int nOfChannels = twoDRegisterAccessor_.getNChannels();
    // if (std::distance(firstSequenceItem, catalogue.end()) < nOfChannels) {
    //     throw InternalErrorException(
    //         "Cannot determine proper multiplexed area and sequence descriptors."
    //     );
    // }

    for (int i = 0; i < nOfChannels; ++i) {
        DeviceElementQTreeItem * sequenceItem = new NumericAddressedCookedSequenceRegisterQTreeItem(registerInfo, twoDRegisterAccessor_, i, this, propertiesWidget);
    }

    mtca4u::RegisterInfoMap::RegisterInfo * numericAddressedRegisterInfo = dynamic_cast<mtca4u::RegisterInfoMap::RegisterInfo *>(registerInfo.get());

    if (numericAddressedRegisterInfo) {
        QString registerName = QString(numericAddressedRegisterInfo->getRegisterName().getComponents().back().c_str());
        QString moduleName = QString(numericAddressedRegisterInfo->getRegisterName().getComponents().front().c_str());
        QString bar = QString::number(numericAddressedRegisterInfo->bar);
        QString nOfElements = QString("");
        QString address = QString::number(numericAddressedRegisterInfo->address);
        QString size = QString::number(numericAddressedRegisterInfo->nBytes);
        QString width = QString("");
        QString fracBits = QString("");
        QString signFlag = QString("");

        properties_ = new RegisterPropertiesWidget::RegisterProperties(registerName, 
        moduleName, bar, address, nOfElements, size, width, fracBits, signFlag);

    } else {
        // FIXME: the cast was invalid, we have assigned wrong DeviceElementQTreeItem.
    }
}

void NumericAddressedCookedMultiplexedAreaQTreeItem::read() {
throw InvalidOperationException(
      "You cannot read from a multiplexed area. Select a sequence register.");
}

void NumericAddressedCookedMultiplexedAreaQTreeItem::write() {
  throw InvalidOperationException(
      "You cannot write to a multiplexed area. Select a sequence register.");
}

void NumericAddressedCookedMultiplexedAreaQTreeItem::updateRegisterProperties() {
    propertiesWidget_->setRegisterProperties(*properties_);
    propertiesWidget_->setFixedPointConverter(nullptr);
}