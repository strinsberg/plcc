#ifndef PLCC_EXCEPTIONS_H
#define PLCC_EXCEPTIONS_H

#include <stdexcept>

/**
* Exception for type error.
*/
class type_error: public std::runtime_error {
 public:
  /**
  * @param errMessage An error message.
  */
  explicit type_error(const char* errMessage) :
    std::runtime_error(errMessage) {}
};

/**
* Exception for errors that cannot be recovered from.
*/
class fatal_error: public std::runtime_error {
 public:
  /**
  * @param errMessage An error message.
  */
  explicit fatal_error(const char* errMessage) :
    std::runtime_error(errMessage) {}
};

#endif
