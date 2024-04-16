#ifndef REGISTER_TYPE_ABSTRACTOR_H
#define REGISTER_TYPE_ABSTRACTOR_H

#include <ChimeraTK/Device.h>
#include <ChimeraTK/SupportedUserTypes.h>

#include <memory>
#include <QVariant>

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

  virtual void setFromOther(RegisterTypeAbstractor const& other) = 0;

  virtual bool isIntegral() const = 0;
  virtual ChimeraTK::DataType rawDataType() const = 0;

  virtual bool readLatest() = 0;
  virtual void read() = 0;
  virtual void write() = 0;

  virtual bool isWritable() = 0;
  virtual bool isReadable() = 0;
  virtual void interrupt() = 0;
  virtual bool hasWaitForNewData() const = 0;

  virtual ChimeraTK::VersionNumber getVersionNumber() = 0;

  virtual ~RegisterTypeAbstractor() = default;
};

///@attention This function can return a nullptr as first pair element in case the data type is
/// undefined (or noData)
std::pair<std::shared_ptr<RegisterTypeAbstractor>, std::shared_ptr<RegisterTypeAbstractor>> createAbstractAccessors(
    ChimeraTK::RegisterInfo const& registerInfo, ChimeraTK::Device& device);

#endif // REGISTER_TYPE_ABSTRACTOR_H
