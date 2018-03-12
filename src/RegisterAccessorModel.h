#ifndef QTHARDMON_REGISTER_ACCESSOR_MODEL_H
#define QTHARDMON_REGISTER_ACCESSOR_MODEL_H

#include <QAbstractTableModel>
//#include <QString>
#include <ChimeraTK/Device.h>

//template <class DATA_TYPE>
class RegisterAccessorModel : public QAbstractTableModel
{
  Q_OBJECT
public:
    RegisterAccessorModel(QObject *parent, ChimeraTK::TwoDRegisterAccessor<int32_t> accessor);
    int rowCount(const QModelIndex &modelIndex = QModelIndex()) const override ;
    int columnCount(const QModelIndex &modelIndex = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    //    Qt::ItemFlags flags(const QModelIndex & index) const override ;

public slots:
    //void setChannel(unsigned int channelNumber);
    void read();
    //    void write();
    
 protected:
    ChimeraTK::TwoDRegisterAccessor<int32_t> _accessor;
    
    //signals:
    //    void editCompleted(const QString &);
};

////template <class DATA_TYPE>
////int RegisterAccessorModel<DATA_TYPE>::rowCount(const QModelIndex &modelIndex) const{
//int RegisterAccessorModel::rowCount(const QModelIndex &modelIndex) const{
//  if (_accessor.isInitialised()){
//    return _accessor.getNElementsPerChannel();
//  }else{
//    return 0;
//  }
//}
//
////template <class DATA_TYPE>
////int RegisterAccessorModel<DATA_TYPE>::columnCount(const QModelIndex & /*modelIndex*/) const{
//int RegisterAccessorModel::columnCount(const QModelIndex & /*modelIndex*/) const{
//  return 1;
//}
//
////template <class DATA_TYPE>
////QVariant RegisterAccessorModel<DATA_TYPE>::data(const QModelIndex &modelIndex, int role) const{
//QVariant RegisterAccessorModel::data(const QModelIndex &modelIndex, int role) const{
//    if (role == Qt::DisplayRole){
//      return QString::number(_accessor[0][modelIndex.column()]);
//    }
//    return QVariant();
//}
//
#endif // QTHARDMON_REGISTER_ACCESSOR_MODEL_H
