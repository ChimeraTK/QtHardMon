/*
 * HexSpinBox.cc
 *
 *  Created on: Feb 12, 2015
 *      Author: varghese
 */

#include "HexSpinBox.h"

#include "CustomDelegates.h"

class CustomDelegates;

HexSpinBox::HexSpinBox(QWidget* parent)
    : QSpinBox(parent), MAX_VALUE(CustomDelegates::MAX_VALUE) {
  // right now the hex spin box accepts [1,7] characters
  // TODO: Make the number of hex characters accepted in line with the range
  // (MIN_VALUE, MAX_VALUE)value supported by the table widget
  QString regex("[0-9A-Fa-f]{1,8}");
  validator = new QRegExpValidator(QRegExp(regex), this);
}

HexSpinBox::~HexSpinBox() { delete validator; }

QString HexSpinBox::textFromValue(int value) const {
  return QString::number(value, 16);
}

int HexSpinBox::valueFromText(const QString& text) const {
  bool conversionStatus = false;
  int convertedValue = text.toUInt(&conversionStatus, 16);
  return convertedValue;
}

QValidator::State HexSpinBox::validate(QString& text, int& pos) const {
  // TODO: Make the number of hex characters accepted in line with the range
  // (MIN_VALUE, MAX_VALUE)value supported by the table widget
  return (validator->validate(text, pos));
}
