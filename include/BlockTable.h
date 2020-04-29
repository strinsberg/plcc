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
  bool new_type(std::string type_name, std::vector<std::shared_ptr<Id>> fields);
  bool has_type(std::string type_name);
  std::vector<std::shared_ptr<Id>>& type_info(std::string type_name);
  void push_block();
  void pop_block();
  int get_level();

  void print();

 private:
  std::vector<std::map<std::string, std::shared_ptr<Id>>> blocks;
  std::vector<std::map<std::string, std::vector<std::shared_ptr<Id>>>> types;
  int level;
};

#endif


