#include "BlockTable.h"
#include "Exprs.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
using namespace std;


BlockTable::BlockTable() : level(0) {}

BlockTable::~BlockTable() {}


bool BlockTable::put(string lexeme, shared_ptr<Id> id) {
  auto it = blocks.back().find(lexeme);
  if ( it != blocks.back().end() )
    return false;

  blocks.back()[lexeme] = id;
  return true;
}


shared_ptr<Id> BlockTable::get(std::string lexeme) {
  for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
    auto item = it->find(lexeme);
    if ( item != it->end() )
      return item->second;
  }
  return nullptr;
}


void BlockTable::push_block() {
  blocks.push_back( map<string, shared_ptr<Id>>() );
  level++;
}


void BlockTable::pop_block() {
  blocks.pop_back();
  level--;
}


int BlockTable::get_level() {
  return level;
}


void BlockTable::print() {
  int i = 1;
  for (auto & blk : blocks) {
    cout << "Level: " << i++ << endl;
    for (auto & it : blk) {
      cout << it.first << endl;
    }
  }
}

