// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_BLOCK_H_
#define STORAGE_LEVELDB_TABLE_BLOCK_H_

#include <stddef.h>
#include <stdint.h>
#include "leveldb/iterator.h"

namespace leveldb {

struct BlockContents;
class Comparator;

/************************
 * Block 是组成sstable的基本单元, Block分为data block, meta block, meta_index block, index block
 * data block用于存放排序的kv，其它block用于索引查找data blcok
 ************************/
class Block {
 public:
  // Initialize the block with the specified contents.
  // Block 只能通过BlockContent进行初始化, BlockContent一般从sstable文件中读取出来
  explicit Block(const BlockContents& contents);

  ~Block();

  size_t size() const { return size_; }
  Iterator* NewIterator(const Comparator* comparator);

 private:
  uint32_t NumRestarts() const;

  const char* data_;
  size_t size_;
  // restart array在data_中的位置
  uint32_t restart_offset_;     // Offset in data_ of restart array
  bool owned_;                  // Block owns data_[]

  // No copying allowed
  Block(const Block&);
  void operator=(const Block&);

  /******************
   * Block内部Iterator，用于迭代block中的kv，
   * 为上层table应用使用
   ******************/
  class Iter;
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_BLOCK_H_
