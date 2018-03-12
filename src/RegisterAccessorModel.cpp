#include "RegisterAccessorModel.h"

RegisterAccessorModel::RegisterAccessorModel(QObject *parent, ChimeraTK::TwoDRegisterAccessor<int32_t> accessor):
  _accessor(accessor)
{
}


int RegisterAccessorModel::rowCount(const QModelIndex &modelIndex) const{
  if (_accessor.isInitialised()){
    return _accessor.getNElementsPerChannel();
  }else{
    return 0;
  }
}

//template <class DATA_TYPE>
//int RegisterAccessorModel<DATA_TYPE>::columnCount(const QModelIndex & /*modelIndex*/) const{
int RegisterAccessorModel::columnCount(const QModelIndex & /*modelIndex*/) const{
  return 1;
}

//template <class DATA_TYPE>
//QVariant RegisterAccessorModel<DATA_TYPE>::data(const QModelIndex &modelIndex, int role) const{
QVariant RegisterAccessorModel::data(const QModelIndex &modelIndex, int role) const{
    if (role == Qt::DisplayRole){
      return QString::number(_accessor[0][modelIndex.row()]);
    }
    return QVariant();
}

void RegisterAccessorModel::read(){
  _accessor.read();
  emit dataChanged(createIndex(0,0),createIndex(rowCount()-1, columnCount()-1));
}
