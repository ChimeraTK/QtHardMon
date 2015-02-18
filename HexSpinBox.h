/*
 * HexSpinBox.h
 *
 *  Created on: Feb 12, 2015
 *      Author: varghese
 */

#ifndef SOURCE_DIRECTORY__HEXSPINBOX_H_
#define SOURCE_DIRECTORY__HEXSPINBOX_H_

#include <qspinbox.h>
#include <qvalidator.h>

class HexData{
  static const int dataType = 0;
public:
  double value;
  HexData(): value(0){};
};

Q_DECLARE_METATYPE(HexData);

class HexSpinBox : public QSpinBox {
  Q_OBJECT
  QRegExpValidator* validator;
  const int MAX_VALUE;
public:
  HexSpinBox (QWidget *parent = 0);
  ~HexSpinBox ();
private:
  QString 	textFromValue ( int value ) const;
  int 	valueFromText ( const QString & text ) const;
  QValidator::State 	validate ( QString & text, int & pos ) const;
};

#endif /* SOURCE_DIRECTORY__HEXSPINBOX_H_ */
