#include "PreferencesProvider.h"

QVariant PreferencesProvider::getRawValue(QString key) {
  if (settings_.count(key) == 0) {
    throw InvalidOperationException(
        std::string("No setting available under \"") + key.toStdString() +
        "\" key.");
  }

  return settings_[key];
}