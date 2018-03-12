#include "RegisterAccessorModel.h"

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
    if (role == Qt::DisplayRole){
      return _abstractAccessor->data(0,modelIndex.row());
    }
    return QVariant();
}

void RegisterAccessorModel::read(){
  _abstractAccessor->read();
  emit dataChanged(createIndex(0,0),createIndex(rowCount()-1, columnCount()-1));
}
