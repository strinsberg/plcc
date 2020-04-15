#ifndef PLCC_ADMIN_H
#define PLCC_ADMIN_H

#include "AstNode.h"
#include "Actions.h"


class Admin {
 public:
  Admin();
  ~Admin();

  AstNode* parse();

 private:
  Actions* act;
};

#endif
