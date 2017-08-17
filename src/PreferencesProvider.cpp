#include "PreferencesProvider.h"

QVariant PreferencesProvider::getRawValue(QString key) {
    if (settings_.count(key) == 0) {
        throw InvalidOperationException (
            "No setting available under requested key."
        );
    }

    return settings_[key];
}