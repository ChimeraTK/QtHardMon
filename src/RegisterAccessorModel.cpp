#include "RegisterAccessorModel.h"

#include <ChimeraTK/SupportedUserTypes.h>

#include <QBrush>
using namespace ChimeraTK;

#include "PreferencesProvider.h"

RegisterAccessorModel::RegisterAccessorModel(QObject* parent,
    std::shared_ptr<RegisterTypeAbstractor> const& abstractAccessor,
    std::shared_ptr<RegisterTypeAbstractor> const& dummyWriteableAccessor)
: QAbstractTableModel(parent), _abstractAccessor(abstractAccessor), _dummyWritableAccessor(dummyWriteableAccessor),
  _channelNumber(0), _cookedHexColumnIndex(-1), _rawColumnIndex(-1), _rawHexColumnIndex(-1), _nColumns(1) {
  _isModified.resize(_abstractAccessor->nChannels());
  for(auto& channelModifiedFlags : _isModified) {
    channelModifiedFlags.resize(_abstractAccessor->nElements());
    // Set all entries to modified to flag the 0s of a write-only register (indicate that
    // these are not the values on the device).
    // The (automatic) read for normal registers will clear it, so there is no behaviour change
    // for readable registers.
    for(auto modifiedFlag : channelModifiedFlags) {
      modifiedFlag = true;
    }
  }
  // show hexadecimal representation for integer data
  // notice: DataType::isIntegral can only be true if the data type is not none.
  // we don't have to check this separately
  if(_abstractAccessor->isIntegral() && !_abstractAccessor->rawDataType().isIntegral()) { // there will be no raw hex
                                                                                          // column
    _cookedHexColumnIndex = _nColumns++;
  }
  if(_abstractAccessor->rawDataType() != DataType::none) {
    // add a raw column if there is a raw data type
    _rawColumnIndex = _nColumns++;
    if(_abstractAccessor->rawDataType().isIntegral()) {
      // add a raw hex column if the raw data type is an integer
      _rawHexColumnIndex = _nColumns++;
    }
  }
}

int RegisterAccessorModel::rowCount(const QModelIndex& /*modelIndex*/) const {
  if(_abstractAccessor) {
    return _abstractAccessor->nElements();
  }
  else {
    return 0;
  }
}

int RegisterAccessorModel::columnCount(const QModelIndex& /*modelIndex*/) const {
  return _nColumns;
}

QVariant RegisterAccessorModel::data(const QModelIndex& modelIndex, int role) const {
  switch(role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      if(modelIndex.row() >= PreferencesProviderSingleton::Instance().getValue<int>("maxWords")) {
        // If the row is larger than maxWords show "truncated"
        // If the size of the accessor is set correctly, it should be maxWords+1,
        // so there should be only one line with this content.
        return QString("truncated");
      }
      if(modelIndex.column() == _cookedHexColumnIndex) {
        return _abstractAccessor->dataAsHex(_channelNumber, modelIndex.row());
      }
      else if(modelIndex.column() == _rawColumnIndex) {
        return _abstractAccessor->rawData(_channelNumber, modelIndex.row());
      }
      else if(modelIndex.column() == _rawHexColumnIndex) {
        return _abstractAccessor->rawDataAsHex(_channelNumber, modelIndex.row());
      }
      else {
        return _abstractAccessor->data(_channelNumber, modelIndex.row());
      }
    case Qt::BackgroundRole:
      if(_isModified[_channelNumber][modelIndex.row()]) { // change background color for modified
                                                          // rows
        return QBrush(QColor(255, 100, 100, 255));        // a light red
      }
      break;
  }
  return QVariant();
}

Qt::ItemFlags RegisterAccessorModel::flags(const QModelIndex& modelIndex) const {
  return Qt::ItemIsEditable | QAbstractTableModel::flags(modelIndex);
}

bool RegisterAccessorModel::setData(const QModelIndex& modelIndex, const QVariant& value, int role) {
  if(role == Qt::EditRole) {
    bool settingSuccessful;
    if((modelIndex.column() == _rawColumnIndex) || (modelIndex.column() == _rawHexColumnIndex)) {
      settingSuccessful = _abstractAccessor->setRawData(_channelNumber, modelIndex.row(), value);
    }
    else {
      settingSuccessful = _abstractAccessor->setData(_channelNumber, modelIndex.row(), value);
    }

    if(settingSuccessful) {
      // setting worked, set the modified flag
      _isModified[_channelNumber][modelIndex.row()] = true;
      return true;
    }
    else {
      return false; // setting not successful
    }
  }
  return false;
}

void RegisterAccessorModel::setChannelNumber(unsigned int channelNumber) {
  _channelNumber = channelNumber;
  emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, columnCount() - 1));
}

bool RegisterAccessorModel::read(bool doBlockingRead) {
  bool hasNewData;
  if(doBlockingRead) {
    _abstractAccessor->read();
    hasNewData = true;
  }
  else {
    hasNewData = _abstractAccessor->readLatest();
  }
  if(hasNewData) {
    clearModifiedFlags();
    emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, columnCount() - 1));
  }
  return hasNewData;
}

bool RegisterAccessorModel::isReadable() {
  return _abstractAccessor->isReadable();
}

void RegisterAccessorModel::write() {
  if(_dummyWritableAccessor) {
    _dummyWritableAccessor->setFromOther(*_abstractAccessor);
    _dummyWritableAccessor->write();
  }
  else {
    _abstractAccessor->write();
  }

  clearModifiedFlags();
  // emit a data changed to clear the red background colours
  emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, columnCount() - 1));
}

void RegisterAccessorModel::clearModifiedFlags() {
  // note: although we modify f we don't have to/can't specify it as auto & f
  // because channelFlags is a vector<bool> which returns a BitReference object
  // which allows to modify it.
  for(auto& channelFlags : _isModified) {
    for(auto f : channelFlags) {
      f = false;
    }
  }
}

QVariant RegisterAccessorModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if(role == Qt::DisplayRole) {
    if(orientation == Qt::Horizontal) {
      if(section == 0) {
        if(_cookedHexColumnIndex != -1) { // there is a cooked hex column, so add (dec)
          return QString("Value (dec)");
        }
        else {
          return QString("Value");
        }
      }
      if(section == _cookedHexColumnIndex) {
        return QString("Value (hex)");
      }
      if(section == _rawColumnIndex) {
        if(_rawHexColumnIndex != -1) { // there is a raw hex column, so add (dec)
          return QString("Raw (dec)");
        }
        else {
          return QString("Raw");
        }
      }
      if(section == _rawHexColumnIndex) {
        return QString("Raw (hex)");
      }
      return QString::number(section);
    }

    if(orientation == Qt::Vertical) {
      return QString::number(section);
    }
  }
  return QVariant();
}

void RegisterAccessorModel::interrupt() {
  _abstractAccessor->interrupt();
}

QDateTime RegisterAccessorModel::getTimeStamp() {
  auto timeStamp = _abstractAccessor->getVersionNumber().getTime();
  auto timeStampIn_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeStamp.time_since_epoch()).count();
  QDateTime retVal;
  retVal.setMSecsSinceEpoch(timeStampIn_ms);
  return retVal;
}

bool RegisterAccessorModel::hasWaitForNewData() {
  return _abstractAccessor->hasWaitForNewData();
}
