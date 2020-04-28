#ifndef PLCC_LITERALS_H
#define PLCC_LITERALS_H

#include "TreeWalker.h"
#include <iostream>
#include <string>


class Literal {
 public:
  Literal() {}
  virtual ~Literal() {}
  virtual void visit(TreeWalker& walker) = 0;
  virtual void display(std::ostream& out) const = 0;
}


class IntBased : public Literal {
 public:
  Literal(int v) : value(v) {}
  virtual ~Literal() {}
  virtual void visit(TreeWalker& walker) { walker.visit(*this); }
  virtual void display(std::ostream& out) const { out << value << endl; }

  int value;
}

class FloatBased : public Literal {
 public:
  Literal(double v) : value(v) {}
  virtual ~Literal() {}
  virtual void visit(TreeWalker& walker) { walker.visit(*this); }
  virtual void display(std::ostream& out) const { out << value << endl; }

  double value;
}

class StringLiteral : public Literal {
 public:
  Literal(std::string v) : value(v) {}
  virtual ~Literal() {}
  virtual void visit(TreeWalker& walker) { walker.visit(*this); }
  virtual void display(std::ostream& out) const { out << value << endl; }

  std::string value;
}

#endif
