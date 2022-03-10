#ifndef REGISTER_TYPE_ABSTRACTOR_H
#define REGISTER_TYPE_ABSTRACTOR_H

#include <ChimeraTK/Device.h>
#include <ChimeraTK/SupportedUserTypes.h>
#include <QVariant>
#include <memory>

/// Register ChimeraTK::Boolean type with Qt
Q_DECLARE_METATYPE(ChimeraTK::Boolean)

/// Qt objects with signals and slots cannot be template classes.
/// As we need a container for the user-type templated RegisterAccessors,
/// this class is used as a non-templated base class a Qt-compatible interface.
class RegisterTypeAbstractor {
 public:
  virtual unsigned int nChannels() const = 0;
  virtual unsigned int nElements() const = 0;
  virtual QVariant data(unsigned int channelIndex, unsigned int elementIndex) const = 0;
  virtual QVariant dataAsHex(unsigned int channelIndex, unsigned int elementIndex) const = 0;
  virtual bool setData(unsigned int channelIndex, unsigned int elementIndex, const QVariant& value) = 0;
  virtual QVariant rawData(unsigned int channelIndex, unsigned int elementIndex) const = 0;
  virtual QVariant rawDataAsHex(unsigned int channelIndex, unsigned int elementIndex) const = 0;
  virtual bool setRawData(unsigned int channelIndex, unsigned int elementIndex, const QVariant& value) = 0;

  virtual bool isIntegral() const = 0;
  virtual ChimeraTK::DataType rawDataType() const = 0;

  // allowBlockingRead must only be true when called from a separate thread (continuous read thread).
  // Otherwise there is the risk of a GUI freeze.
  virtual void read(bool allowBlockingRead) = 0;
  virtual void write() = 0;

  virtual bool isWritable() = 0;
  virtual bool isReadable() = 0;

  virtual ~RegisterTypeAbstractor() = default;
};

///@attention This function can return a nullptr in case the data type is
/// undefined (or noData)
std::shared_ptr<RegisterTypeAbstractor> createAbstractAccessor(
    ChimeraTK::RegisterInfo const& registerInfo, ChimeraTK::Device& device);

#endif // REGISTER_TYPE_ABSTRACTOR_H
