/*
 * Exceptions.h
 *
 *  Created on: Jul 16, 2015
 *      Author: varghese
 */

#ifndef SOURCE_DIRECTORY__EXCEPTIONS_H_
#define SOURCE_DIRECTORY__EXCEPTIONS_H_

#include <exception>
#include <string>

#define DEFINE_QTHARDMON_EXCEPTION(NAME)                                       \
  class NAME : public Exception {                                              \
  public:                                                                      \
    NAME(std::string const& message) : Exception(message) {}                   \
  };

/**
 * Custom exceptions for the helper methods
 */
class Exception : public std::exception {
protected:
  /// The error message for your exception
  std::string const _message;

public:
  /**
   * Default constructor
   */
  Exception(std::string const& message) : _message(message) {}
  /// overload of the default
  virtual const char* what() const throw() { return _message.c_str(); }

  virtual ~Exception() throw() {}
};

DEFINE_QTHARDMON_EXCEPTION(InternalErrorException)
DEFINE_QTHARDMON_EXCEPTION(InvalidOperationException)

#endif /* SOURCE_DIRECTORY__EXCEPTIONS_H_ */
