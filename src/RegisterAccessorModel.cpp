#include "RegisterAccessorModel.h"
#include <QBrush>
#include <ChimeraTK/SupportedUserTypes.h>
using namespace ChimeraTK;

RegisterAccessorModel::RegisterAccessorModel(QObject *parent, std::shared_ptr<RegisterTypeAbstractor> const & abstractAccessor) :
  _abstractAccessor(abstractAccessor), _channelNumber(0), _coockedHexColumnIndex(-1), _rawColumnIndex(-1), _rawHexColumnIndex(-1), _nColumns(1)
{
  _isModified.resize(_abstractAccessor->nChannels());
  for (auto & channelModifiedFlags: _isModified){
    channelModifiedFlags.resize(_abstractAccessor->nElements());
  }
  // show hexadecimal representation for integer data
  // notice: DataType::isIntegral can only be true if the data type is not none.
  // we don't have to check this separately
  if (  _abstractAccessor->isIntegral() &&
        !_abstractAccessor->rawDataType().isIntegral() ){ // there will be no raw hex column
    _coockedHexColumnIndex = _nColumns++;
  }
  if ( _abstractAccessor->rawDataType() != DataType::none ){
    // add a raw column if there is a raw data type
    _rawColumnIndex = _nColumns++;
    if ( _abstractAccessor->rawDataType().isIntegral() ){
      // add a raw hex column if the raw data type is an integer
      _rawHexColumnIndex = _nColumns++;
    }
  }
}

int RegisterAccessorModel::rowCount(const QModelIndex &modelIndex) const{
  if (_abstractAccessor){
    return _abstractAccessor->nElements();
  }else{
    return 0;
  }
}

int RegisterAccessorModel::columnCount(const QModelIndex & /*modelIndex*/) const{
  return _nColumns;
}

QVariant RegisterAccessorModel::data(const QModelIndex &modelIndex, int role) const{
  switch (role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      //@todo FIXME The current assumtion that column 1 is always the hex column will not hold.
      if (modelIndex.column() == _coockedHexColumnIndex){
        return _abstractAccessor->dataAsHex(_channelNumber,modelIndex.row());
      }else{
        return _abstractAccessor->data(_channelNumber,modelIndex.row());
      }
    case Qt::BackgroundRole:
      if (_isModified[_channelNumber][modelIndex.row()]){  //change background color for modified rows
        return QBrush( QColor( 255, 100, 100, 255 ) ); // a light red
      }
      break;
  }
  return QVariant();
}

Qt::ItemFlags RegisterAccessorModel::flags(const QModelIndex &modelIndex) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(modelIndex);
}

bool RegisterAccessorModel::setData(const QModelIndex & modelIndex, const QVariant & value, int role){
  if (role == Qt::EditRole){
    if ( _abstractAccessor->setData(_channelNumber,modelIndex.row(), value)){
      // setting worked, set the modified flag
      _isModified[_channelNumber][modelIndex.row()] = true;
      return true;
    }else{
      return false; // setting not successful
    }      
  }
  return true;
}

void RegisterAccessorModel::setChannelNumber(unsigned int channelNumber){
  _channelNumber=channelNumber;
  emit dataChanged(createIndex(0,0),createIndex(rowCount()-1, columnCount()-1));
}

void RegisterAccessorModel::read(){
  _abstractAccessor->read();
  clearModifiedFlags();
  emit dataChanged(createIndex(0,0),createIndex(rowCount()-1, columnCount()-1));
}

void RegisterAccessorModel::write(){
  _abstractAccessor->write();
  clearModifiedFlags();
  // emit a data changed to clear the red background colours
  emit dataChanged(createIndex(0,0),createIndex(rowCount()-1, columnCount()-1));
}

void RegisterAccessorModel::clearModifiedFlags(){
  // note: although we modify f we don't have to/can't specify it as auto & f because
  // channelFlags is a vector<bool> which returns a BitReference object which allows to modify it.
  for (auto & channelFlags : _isModified){
    for (auto f : channelFlags){
      f=false;
    }
  }
}

QVariant RegisterAccessorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
      if (orientation == Qt::Horizontal){
        switch (section){
          case 0:
            return QString("Value")+(_abstractAccessor->isIntegral()?" (dec)":"");
          case 1:
            return QString("Value (hex)");
          default:
            return QString::number(section);
        }            
      }
      if (orientation == Qt::Vertical){
        return QString::number(section);
      }
    }
    return QVariant();
}
