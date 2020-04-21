#include "TreeWalker.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include <iostream>
#include <memory>
using namespace std;


TreeWalker::TreeWalker() {}

TreeWalker::~TreeWalker() {}

void TreeWalker::walk(AstNode& tree) {
  cout << tree << endl;
}

