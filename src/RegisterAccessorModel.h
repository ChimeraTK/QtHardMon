#ifndef QTHARDMON_REGISTER_ACCESSOR_MODEL_H
#define QTHARDMON_REGISTER_ACCESSOR_MODEL_H

#include "RegisterTypeAbstractor.h"
#include <ChimeraTK/Device.h>
#include <QAbstractTableModel>
#include <QDateTime>

class RegisterAccessorModel : public QAbstractTableModel {
  Q_OBJECT
 public:
  RegisterAccessorModel(QObject* parent, std::shared_ptr<RegisterTypeAbstractor> const& abstractAccessor);
  int rowCount(const QModelIndex& modelIndex = QModelIndex()) const override;
  int columnCount(const QModelIndex& modelIndex = QModelIndex()) const override;
  QVariant data(const QModelIndex& modelIndex, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& modelIndex, const QVariant& value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex& modelIndex) const override;
  bool isReadable();

  void setChannelNumber(unsigned int channelNumber);

  bool readLatest(); // returns whether new data is available
  void write();
  QDateTime getTimeStamp();

  void interrupt();
  bool hasWaitForNewData();

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

 protected:
  std::shared_ptr<RegisterTypeAbstractor> _abstractAccessor;
  std::vector<std::vector<bool>> _isModified; // remember if a row has been modified per channel

  void clearModifiedFlags(); // clear the modified flags
  unsigned int _channelNumber;
  // If any of the follogin columns is there depends on the register.
  // If the column is not there, the index is set to -1.
  // Currently implemented scenarios
  // (as examples at the time of writing. There might be more when you read this
  // ;-) )
  //
  // * Just 1 column cooked (double or string) This column is allways there and
  // the first one,
  //   so we don't need an indicator for it.
  // * Only cooked integer as decimal and hex
  // * Cooked double and integer raw (dec and hex)
  // * Cooked and raw integer. In this case the cooked hex is not shown (only
  // raw hex)
  int _cookedHexColumnIndex;
  int _rawColumnIndex; // Usually a decimal value, but might in future be text
                       // or floating point
  int _rawHexColumnIndex;

  int _nColumns;
};

#endif // QTHARDMON_REGISTER_ACCESSOR_MODEL_H
