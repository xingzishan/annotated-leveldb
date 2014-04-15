// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_

#include <vector>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace leveldb {

/********************************
 * 类似memcache中的slab机制，分配一块空间(4K)，比block小的空间直接在
 * block上分配，block空间不足的话重新分配一个block或者直接分配申请空间
 * 减少内存碎片
 *******************************/
class Arena {
 public:
  Arena();
  ~Arena();

  // Return a pointer to a newly allocated memory block of "bytes" bytes.
  char* Allocate(size_t bytes);

  // Allocate memory with the normal alignment guarantees provided by malloc
  /*****************************
   * 按8B字节对齐分配内存
   ****************************/
  char* AllocateAligned(size_t bytes);

  // Returns an estimate of the total memory usage of data allocated
  // by the arena (including space allocated but not yet used for user
  // allocations).
  size_t MemoryUsage() const {
    return blocks_memory_ + blocks_.capacity() * sizeof(char*);
  }

 private:
  char* AllocateFallback(size_t bytes);
  char* AllocateNewBlock(size_t block_bytes);

  // Allocation state
  /***********************
   * 分配状态，标示当前block可用空间地址以及剩余空间
   **********************/
  char* alloc_ptr_;
  size_t alloc_bytes_remaining_;

  // Array of new[] allocated memory blocks
  /***********************
   * 内存按块分配，分配的块放入vector中，blocks存储实际配分的内存空间
   ***********************/
  std::vector<char*> blocks_;

  // Bytes of memory in blocks allocated so far
  size_t blocks_memory_;

  // No copying allowed
  Arena(const Arena&);
  void operator=(const Arena&);
};

/****************************
 * 如果当前block空间足够，直接在当前block上分配
 * 否则重新申请block或者直接分配申请的空间大小
 ***************************/
inline char* Arena::Allocate(size_t bytes) {
  // The semantics of what to return are a bit messy if we allow
  // 0-byte allocations, so we disallow them here (we don't need
  // them for our internal use).
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaining_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }
  return AllocateFallback(bytes);
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_UTIL_ARENA_H_
