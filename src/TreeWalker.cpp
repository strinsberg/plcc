#include "TreeWalker.h"
#include "AstNode.h"
#include <iostream>
#include <memory>
using namespace std;


TreeWalker::TreeWalker() {}

TreeWalker::~TreeWalker() {}

void TreeWalker::walk(AstNode& tree) {
  tree.visit(*this);
}

void TreeWalker::visit(AstNode& node) {
  std::cout << node << std::endl;
}
