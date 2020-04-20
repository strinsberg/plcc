#include "Parser.h"
#include "Admin.h"
#include "AstNode.h"
#include <iostream>
#include <memory>

int main() {
  auto admin = std::make_shared<Admin>(true);
  Parser p(admin);
  std::shared_ptr<AstNode> ast = p.parse();

  std::cout << std::endl;
  std::cout << "==== Syntax Tree ====" << std::endl;
  TreeWalker walker;
  walker.walk(*ast);

  return 0;
}
