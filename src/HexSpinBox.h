#ifndef SOURCE_DIRECTORY__HEXSPINBOX_H_
#define SOURCE_DIRECTORY__HEXSPINBOX_H_

#include <QSpinBox>
#include <QValidator>
// The HexDataType is defined in a common file together with other types to
// avoid collisions (although at the time of writing there are no other types)
#include "VariantUserTypes.h"

/**
 * The HexData class is the custom datatype that we have introduced to represent
 * hex values. This class is registered with the Qt framework as a user defined
 * type. The table spinbox delegate class uses the class to identify hex data
 * items and create HexSpinboxes accordingly
 */
class HexData {
  static const int dataType = 0;

public:
  /**
   * Holds the entered value (for which we need the hex representation)
   */
  double value;
  HexData(double v = 0) : value(v){};
  /** Overloaded constructor for std::string to avoid template specialisation as
   *  there is no implicit conversion from std::string to double.
   *  Generally this conversion does not make sense, so we initialise class with
   * 0.
   */
  HexData(std::string v) : value(0){};
};

/*
 * Register our custom type with the Qt Framework
 */
Q_DECLARE_METATYPE(HexData)

/**
 * This class defines the properties of the spinbox that is used to accept the
 * hex values from the user.
 */
class HexSpinBox : public QSpinBox {
  Q_OBJECT
  QRegExpValidator *_validator;

public:
  /**
   * The class can register itself with a parent widget through the constructor.
   */
  HexSpinBox(QWidget *parent_ = 0);

private:
  // Disable copy constructor and assignment operator
  Q_DISABLE_COPY(HexSpinBox)
  ~HexSpinBox();

protected:
  /**
   * This method defines the displayed representation of the value in the
   * spinbox. The current code displays the value which is internally stored as
   * an integer datatype as the corresponding hexadecimal string.
   */
  QString textFromValue(int value_) const;

  /**
   * Defines how the user input text (in the spinbox) is converted into the
   * corresponding integer value
   */
  int valueFromText(const QString &text_) const;

  /**
   * Code is used to restrict user input (in the spinbox) to valid hexadecimal
   * characters.
   */
  QValidator::State validate(QString &text_, int &pos_) const;
};

#endif /* SOURCE_DIRECTORY__HEXSPINBOX_H_ */
