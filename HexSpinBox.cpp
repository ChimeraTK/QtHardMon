/*
 * HexSpinBox.cc
 *
 *  Created on: Feb 12, 2015
 *      Author: varghese
 */

#include "HexSpinBox.h"
#include <sstream>


HexSpinBox::HexSpinBox(QWidget* parent)
    : QSpinBox(parent) {
  // right now the hex spin box accepts [1,7] characters
  QString regex("[0-9A-Fa-f]{1,8}");
  validator = new QRegExpValidator(QRegExp(regex), this);
}

HexSpinBox::~HexSpinBox() { delete validator; }

QString HexSpinBox::textFromValue(int value) const {
  // QString::number has a bug and gives negative values as 64 bit hex.
  // So we have to do it manually.
  std::stringstream s;
  s << std::hex << value;
  return QString(s.str().c_str());
}

int HexSpinBox::valueFromText(const QString& text) const {
  bool conversionStatus = false;
  int convertedValue = text.toUInt(&conversionStatus, 16);
  return convertedValue;
}

QValidator::State HexSpinBox::validate(QString& text, int& pos) const {
  // TODO: Make the number of hex characters accepted in line with the range
  return (validator->validate(text, pos));
}
