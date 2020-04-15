#ifndef PLCC_TOKENS_H
#define PLCC_TOKENS_H

#include "Symbol.h"
#include <string>


class Token {
 public:
  Token(tag::Tag t) : tag(t) {}
  virtual ~Token() {}
  virtual std::string to_string() { return tag::to_string.at(tag); }
  tag::Tag tag;
};


class Number : public Token {
 public:
  Number(int v) : Token(tag::INT), value(v) {}
  virtual ~Number() {}
  virtual std::string to_string() { return std::to_string(value); }
  int value;
};


class Word : public Token {
 public:
  Word(const std::string s) : Token(tag::NAME), lexeme(s) {}
  virtual ~Word() {}
  virtual std::string to_string() { return lexeme; }
  std::string lexeme;
};


class Float : public Token {
 public:
  Float(int v, int d) : Token(tag::FLOAT), value(v), decimal(d) {}
  virtual ~Float() {}
  virtual std::string to_string() {
    return std::to_string(value) + "." + std::to_string(decimal); }
  int value, decimal;
};


class Char : public Token {
 public:
  Char(char v) : Token(tag::CHAR), value(v) {}
  virtual ~Char() {}
  virtual std::string to_string() { return std::string(1, value); }
  char value;
};

#endif

