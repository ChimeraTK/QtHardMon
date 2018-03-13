#include "RegisterAccessorModel.h"
#include <QBrush>

RegisterAccessorModel::RegisterAccessorModel(QObject *parent, std::shared_ptr<RegisterTypeAbstractor> const & abstractAccessor) :
  _abstractAccessor(abstractAccessor)
{
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
      if (modelIndex.row()%2){  //change background for testing
        return QBrush( QColor( 255, 100, 100, 255 ) ); // a light red
      }
      break;
  }
  return QVariant();
}

void RegisterAccessorModel::read(){
  _abstractAccessor->read();
  emit dataChanged(createIndex(0,0),createIndex(rowCount()-1, columnCount()-1));
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
