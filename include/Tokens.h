#ifndef PLCC_TOKENS_H
#define PLCC_TOKENS_H

#include <string>
#include "parser.tab.h"


class Token {
 public:
  Token(yytokentype t) : tag(t) {}
  virtual ~Token() {}
  virtual std::string to_string() { return std::to_string((int)tag); }
  yytokentype tag;
};

class Number : public Token {
 public:
  Number(yytokentype t, int v) : Token(t), value(v) {}
  virtual ~Number() {}
  virtual std::string to_string() { return std::to_string(value); }
  int value;
};


class Word : public Token {
 public:
  Word(yytokentype t, std::string s) : Token(t), lexeme(s) {}
  virtual ~Word() {}
  virtual std::string to_string() { return lexeme; }
  std::string lexeme;
};

class Float : public Token {
 public:
  Float(yytokentype t, double v) : Token(t), value(v) {}
  virtual ~Float() {}
  virtual std::string to_string() { return std::to_string(value); }
  double value;
};

#endif

