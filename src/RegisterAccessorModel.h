#ifndef QTHARDMON_REGISTER_ACCESSOR_MODEL_H
#define QTHARDMON_REGISTER_ACCESSOR_MODEL_H

#include <QAbstractTableModel>
#include <ChimeraTK/Device.h>
#include "RegisterTypeAbstractor.h"

class RegisterAccessorModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  RegisterAccessorModel(QObject *parent, std::shared_ptr<RegisterTypeAbstractor> const & abstractAccessor);
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
    std::shared_ptr<RegisterTypeAbstractor> _abstractAccessor;
    
    //signals:
    //    void editCompleted(const QString &);
};

#endif // QTHARDMON_REGISTER_ACCESSOR_MODEL_H
