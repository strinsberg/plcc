#include "Parser.h"
#include "Admin.h"
#include "AstNode.h"
#include <iostream>

int main() {
  Admin a(true);
  Parser p(&a);
  AstNode* ast = p.parse();

  std::cout << std::endl;
  std::cout << "==== Syntax Tree ====" << std::endl;
  std::cout << *ast << std::endl;

  return 0;
}
