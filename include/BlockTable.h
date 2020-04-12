#ifndef PLCC_BLOCK_TABLE_H
#define PLCC_BLOCK_TABLE_H

#include "Exprs.h"
#include <string>
#include <map>
#include <vector>


class BlockTable {
 public:
  BlockTable();
  ~BlockTable();

  bool put(std::string lexeme, Id* id);
  Id* get(std::string lexeme);
  void push_block();
  void pop_block();
  int get_level();

  void print();

 private:
  std::vector<std::map<std::string, Id*>> blocks;
  int level;

  void del_block(std::map<std::string, Id*>& block);
};

#endif


