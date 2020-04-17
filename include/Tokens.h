#ifndef PLCC_TOKENS_H
#define PLCC_TOKENS_H

#include "Symbol.h"
#include <string>


// Get rid of all token classes in favour of types, operators, and constant exprs
class Token {
 public:
  Token(symbol::Tag t) : tag(t) {}
  virtual ~Token() {}
  virtual std::string to_string() { return symbol::to_string.at(tag); }
  symbol::Tag tag;
};


class Number : public Token {
 public:
  Number(int v) : Token(symbol::INT), value(v) {}
  virtual ~Number() {}
  virtual std::string to_string() { return std::to_string(value); }
  int value;
};


class Word : public Token {
 public:
  Word(const std::string s) : Token(symbol::NAME), lexeme(s) {}
  virtual ~Word() {}
  virtual std::string to_string() { return lexeme; }
  std::string lexeme;
};


class Float : public Token {
 public:
  Float(int v, int d) : Token(symbol::FLOAT), value(v), decimal(d) {}
  virtual ~Float() {}
  virtual std::string to_string() {
    return std::to_string(value) + "." + std::to_string(decimal); }
  int value, decimal;
};


class Char : public Token {
 public:
  Char(char v) : Token(symbol::CHAR), value(v) {}
  virtual ~Char() {}
  virtual std::string to_string() { return std::string(1, value); }
  char value;
};

class Type {
  // should have 3 tags. one for type, kind, and qualifier
};

class Operator {
  // should have a type so that it can be used to match types
};

#endif

