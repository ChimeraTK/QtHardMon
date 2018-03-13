#ifndef REGISTER_TYPE_ABSTRACTOR_H
#define REGISTER_TYPE_ABSTRACTOR_H

#include <QVariant>
#include <memory>
#include <ChimeraTK/Device.h>

///Qt objects with signals and slots cannot be template classes.
///As we need a container for the user-type templated RegisterAccessors,\
///this class is used as a non-templated base class a Qt-compatible interface.
class RegisterTypeAbstractor{
 public:
  virtual unsigned int nChannels() const = 0;
  virtual unsigned int nElements() const = 0;
  virtual QVariant data(unsigned int channelIndex, unsigned int elementIndex) const = 0;

  virtual void read() = 0;
  virtual void write() = 0;
};

std::shared_ptr<RegisterTypeAbstractor> createAbstractAccessor(ChimeraTK::RegisterInfo const & registerInfo, ChimeraTK::Device & device);

#endif // REGISTER_TYPE_ABSTRACTOR_H
