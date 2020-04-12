#include "BlockTable.h"
#include "Exprs.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;


BlockTable::BlockTable() : level(0) {
  push_block();
}

BlockTable::~BlockTable() {
  for (auto & blk : blocks)
    del_block(blk);
}

bool BlockTable::put(string lexeme, Id* id) {
  auto it = blocks.back().find(lexeme);
  if ( it != blocks.back().end() )
    return false;

  blocks.back()[lexeme] = id;
  return true;
}

Id* BlockTable::get(std::string lexeme) {
  auto it = blocks.back().find(lexeme);
  if ( it != blocks.back().end() )
    return it->second;
  return nullptr;
}

void BlockTable::push_block() {
  blocks.push_back( map<string, Id*>() );
  level++;
}

void BlockTable::pop_block() {
  del_block( blocks.back() );
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

void BlockTable::del_block(std::map<std::string, Id*>& block) {
    for (auto & it : block)
      delete it.second;
}
