#include "RegisterAccessorModel.h"
#include <QBrush>

RegisterAccessorModel::RegisterAccessorModel(QObject *parent, std::shared_ptr<RegisterTypeAbstractor> const & abstractAccessor) :
  _abstractAccessor(abstractAccessor)
{
  _isModified.resize(_abstractAccessor->nElements());
}

int RegisterAccessorModel::rowCount(const QModelIndex &modelIndex) const{
  if (_abstractAccessor){
    return _abstractAccessor->nElements();
  }else{
    return 0;
  }
}

int RegisterAccessorModel::columnCount(const QModelIndex & /*modelIndex*/) const{
  return 1;
}

QVariant RegisterAccessorModel::data(const QModelIndex &modelIndex, int role) const{
  switch (role){
    case Qt::DisplayRole:
      return _abstractAccessor->data(0,modelIndex.row());
    case Qt::BackgroundRole:
      if (_isModified[modelIndex.row()]){  //change background color for modified rows
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
    if ( _abstractAccessor->setData(0,modelIndex.row(), value)){
      // setting worked, set the modified flag
      _isModified[modelIndex.row()] = true;
      return true;
    }else{
      return false; // setting not successful
    }      
  }
  return true;
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
  // _isModified is a vector<bool> which returns a BitReference object which allows to modify it.
  for (auto f : _isModified){
    f=false;
  }
}

QVariant RegisterAccessorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
      if (orientation == Qt::Horizontal){
        switch (section){
          case 0:
            return QString("Value");
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
