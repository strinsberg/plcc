#include "BlockTable.h"
#include "Exprs.h"
#include <stdexcept>
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


bool BlockTable::new_type(string type_name, vector<shared_ptr<Id>> fields) {
  if ( has_type(type_name) )
    return false;

  types.back()[type_name] = fields;
  return true;
}


bool BlockTable::has_type(std::string type_name) {
  for (auto it = types.rbegin(); it != types.rend(); ++it) {
    auto fields = it->find(type_name);
    if ( fields != it->end() )
      return true;
  }
  return false;
}


vector<shared_ptr<Id>>& BlockTable::type_info(std::string type_name) {
  for (auto it = types.rbegin(); it != types.rend(); ++it) {
    auto fields = it->find(type_name);
    if ( fields != it->end() )
      return fields->second;
  }
  throw invalid_argument("type does not exist");
}


void BlockTable::push_block() {
  blocks.push_back( map<string, shared_ptr<Id>>() );
  types.push_back( map<string, vector<shared_ptr<Id>>>() );
  level++;
}


void BlockTable::pop_block() {
  blocks.pop_back();
  types.pop_back();
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

