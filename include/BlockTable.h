#ifndef PLCC_BLOCK_TABLE_H
#define PLCC_BLOCK_TABLE_H

#include "Exprs.h"
#include <string>
#include <map>
#include <vector>
#include <memory>


class BlockTable {
 public:
  BlockTable();
  ~BlockTable();

  bool put(std::string lexeme, std::shared_ptr<Id> id);
  std::shared_ptr<Id> get(std::string lexeme);
  void push_block();
  void pop_block();
  int get_level();

  void print();

 private:
  std::vector<std::map<std::string, std::shared_ptr<Id>>> blocks;
  int level;
};

#endif


