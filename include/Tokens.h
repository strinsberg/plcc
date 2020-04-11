#ifdef TOKENS_H
#define TOKENS_H

#include "Tag.h"


class Token {
 public:
  Token(Tag t) : tag(t) {}
  virtual ~Token() {}
  virtual std::string to_string() { return std::to_string((int)tag); }
  Tag tag;
};

class Number : public Token {
 public:
  Number(Tag t, int v) : Token(t), value(v) {}
  virtual ~Number() {}
  virtual std::string to_string() { return std::to_string(value); }
  int value;
};


class Word : public Token {
 public:
  Word(Tag t, std::string s) : Token(t), lexeme(s) {}
  virtual ~Word() {}
  virtual std::string to_string() { return lexeme; }
  std::string lexeme;
};

class Float : public Token {
 public:
  Float(Tag t, double v) : Token(t), value(v) {}
  virtual ~Float() {}
  virtual std::string to_string() { return std::to_string(value); }
  std::string value;
};

#endif

