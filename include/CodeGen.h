#ifndef PLCC_CODE_GEN_H
#define PLCC_CODE_GEN_H

class CodeGen {
 public:
  CodeGen() {}
  virtual ~CodeGen() {}
  // Eventually must have overloads for visiting all AstNode types
  virtual visit(AstNode* node) = 0;
};

#endif
