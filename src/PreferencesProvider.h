#ifndef PREFERENCESPROVIDER_H
#define PREFERENCESPROVIDER_H

#include <map>
#include <QString>
#include <QVariant>
#include "Exceptions.h"
#include <sstream>

class PreferencesProvider {

private:
    std::map<QString, QVariant> settings_;

public:
    template <typename T>
    void setValue(QString key, T value);

    template <typename T>
    T getValue(QString key);

    QVariant getRawValue(QString key);

};

template <typename T>
void PreferencesProvider::setValue(QString key, T value) {
    if (settings_.count(key) != 0) {
        // FIXME: what about implicit casting?
        if (QVariant(value).type() != settings_[key].type()) {
            throw InvalidOperationException (
                "You cannot change the setting type once you insert it."
            );
        }
    }

    settings_[key] = value;
}

template <typename T>
T PreferencesProvider::getValue(QString key) {
    QVariant value = getRawValue(key);
    std::stringstream temporaryStream;

    switch(static_cast<QMetaType::Type>(value.type())) {
        case QMetaType::Int:
            temporaryStream << value.toInt();
            break;
        case QMetaType::Float:
            temporaryStream << value.toFloat();
            break;
        case QMetaType::Double:
            temporaryStream << value.toDouble();
            break;
        case QMetaType::QString:
            temporaryStream << value.toString().toStdString();
            break;
        case QMetaType::Bool:
            temporaryStream << value.toBool();
            break;
        default:
            throw InvalidOperationException (
                "Type not supported. Use getRawValue instead."
            );
    }

    T returnedValue; 
    temporaryStream >> returnedValue;
    return returnedValue;
}

#include "SingletonHolder.h"

typedef SingletonHolder<PreferencesProvider, CreateByNew, LifetimeStandard, SingleThread> PreferencesProviderSingleton;

#endif // PREFERENCESPROVIDER_H