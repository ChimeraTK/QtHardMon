/*
 * HexSpinBox.cc
 *
 *  Created on: Feb 12, 2015
 *      Author: varghese
 */

#include "HexSpinBox.h"
#include "TableSpinBoxDelegate.h"

class TableSpinBoxDelegate;

HexSpinBox::HexSpinBox (QWidget* parent): QSpinBox(parent) , MAX_VALUE(TableSpinBoxDelegate::MAX_VALUE){
  //TODO: Make the number of charachters accepted more accurate
  // right now hardcoded to 14 .
  QString regex("[0-9A-Fa-f]{1,14}");
  validator = new QRegExpValidator(QRegExp(regex), this);
}

HexSpinBox::~HexSpinBox () {
 delete validator;
}

QString
HexSpinBox::textFromValue (int value) const {
return QString::number(value, 16);
}

int
HexSpinBox::valueFromText (const QString& text) const {
  bool conversionStatus = false;
  return (text.toInt(&conversionStatus, 16));
}

QValidator::State
HexSpinBox::validate (QString& text, int& pos) const {
  // TODO: limit the number of chars to the max val supported

  if(validator->validate(text, pos) == QValidator::Acceptable){
    bool conversionStatus = false;
    int enteredValueSoFar = text.toInt(&conversionStatus, 16);
    if(enteredValueSoFar <= MAX_VALUE){
      return QValidator::Acceptable;
    }
  }
    return QValidator::Invalid;

}
