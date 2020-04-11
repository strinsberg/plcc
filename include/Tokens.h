#ifndef PLCC_TOKENS_H
#define PLCC_TOKENS_H

#include "Symbol.h"
#include <string>
#include "parser.tab.h"


class Token {
 public:
  Token(yytokentype t) : tag(t) {}
  virtual ~Token() {}
  virtual std::string to_string() { return tok_string.at(tag); }
  yytokentype tag;
};


class Number : public Token {
 public:
  Number(int v) : Token(INT), value(v) {}
  virtual ~Number() {}
  virtual std::string to_string() { return std::to_string(value); }
  int value;
};


class Word : public Token {
 public:
  Word(const std::string s) : Token(NAME), lexeme(s) {}
  virtual ~Word() {}
  virtual std::string to_string() { return lexeme; }
  std::string lexeme;
};


class Float : public Token {
 public:
  Float(int v, int d) : Token(FLOAT), value(v), decimal(d) {}
  virtual ~Float() {}
  virtual std::string to_string() {
    return std::to_string(value) + "." + std::to_string(decimal); }
  int value, decimal;
};


class Char : public Token {
 public:
  Char(char v) : Token(CHAR), value(v) {}
  virtual ~Char() {}
  virtual std::string to_string() { return std::string(1, value); }
  char value;
};

#endif

