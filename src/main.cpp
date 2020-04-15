#include "Parser.h"
#include "Admin.h"

int main() {
  Admin a(true);
  Parser p(&a);
  p.parse();

  return 0;
}
