#include "AstNode.h"
#include <iostream>
using namespace std;


int AstNode::label = 1;


AstNode::AstNode(int line) : line_num(line) {}


AstNode::~AstNode() {}


int AstNode::new_label() {
  return label++;
}


void AstNode::emit_label(int i) {
  cout << "L" << i << ":" << endl;
}


void AstNode::emit(std::string s) {
  cout << "\t" << s << endl;
}


void AstNode::error(std::string s) {
  cout << "near line " << line_num << ": " << s << endl;
}

