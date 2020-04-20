#ifndef PLCC_ADMIN_H
#define PLCC_ADMIN_H

#include <string>


class Admin {
 public:
  Admin(bool d);
  ~Admin();

  void debug(std::string text);
  void error(std::string text, std::string lexeme="");
  void newline();
  int get_line();
  int get_errors();

 private:
  int line;
  int errors;
  bool line_error;
  bool is_debug;
};

#endif
