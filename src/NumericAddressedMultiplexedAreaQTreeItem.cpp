#include "NumericAddressedMultiplexedAreaQTreeItem.h"
#include "Exceptions.h"
#include "ModuleQTreeItem.h"
#include <iterator>
#include "NumericAddressedSequenceRegisterQTreeItem.h"

NumericAddressedMultiplexedAreaQTreeItem::NumericAddressedMultiplexedAreaQTreeItem(mtca4u::Device & device, boost::shared_ptr<mtca4u::RegisterInfo> registerInfo, const mtca4u::RegisterCatalogue & catalogue, mtca4u::RegisterCatalogue::const_iterator firstSequenceItem, QTreeWidget * parent, RegisterPropertiesWidget * propertiesWidget)
: NumericAddressedElementQTreeItem(device, registerInfo, parent, propertiesWidget),
  twoDRegisterAccessor_(device.getTwoDRegisterAccessor<double>(getTrimmedRegisterName(registerInfo))),
  propertiesWidget_(propertiesWidget)
{
    unsigned int nOfChannels = twoDRegisterAccessor_.getNChannels();
    // if (std::distance(firstSequenceItem, catalogue.end()) < nOfChannels) {
    //     throw InternalErrorException(
    //         "Cannot determine proper multiplexed area and sequence descriptors."
    //     );
    // }

    mtca4u::RegisterCatalogue::const_iterator channelIterator = firstSequenceItem;

    for (int i = 0; i < nOfChannels && channelIterator != catalogue.end(); ++i, ++channelIterator) {
        boost::shared_ptr<mtca4u::RegisterInfo> currentSequenceItem = catalogue.getRegister(channelIterator->getRegisterName());
        DeviceElementQTreeItem * sequenceItem = new NumericAddressedSequenceRegisterQTreeItem(currentSequenceItem, twoDRegisterAccessor_, i, this, propertiesWidget);
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

void NumericAddressedMultiplexedAreaQTreeItem::read() {
throw InvalidOperationException(
      "You cannot read from a multiplexed area. Select a sequence register.");
}

void NumericAddressedMultiplexedAreaQTreeItem::write() {
  throw InvalidOperationException(
      "You cannot write to a multiplexed area. Select a sequence register.");
}

std::string NumericAddressedMultiplexedAreaQTreeItem::getTrimmedRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo) {
  std::string regName = registerInfo->getRegisterName().getComponents().back();
  
  std::size_t prefixFound = regName.find("AREA_MULTIPLEXED_SEQUENCE_");
  if (prefixFound == 0) {
    return registerInfo->getRegisterName().getComponents().front() + "/" + regName.substr(26);
  } else {
      throw InternalErrorException(
      "Cannot determine proper multiplexed area and sequence descriptors.");
  }

}

void NumericAddressedMultiplexedAreaQTreeItem::updateRegisterProperties() {
    propertiesWidget_->setRegisterProperties(*properties_);
    propertiesWidget_->setFixedPointConverter(nullptr);
}