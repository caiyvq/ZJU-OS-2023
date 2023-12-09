#pragma once
#include "types.h"
#include "stdint.h"

struct run {
    struct run *next;
};

void mm_init();

uint64_t kalloc();
void kfree(uint64_t);

struct buddy {
  uint64_t size;
  uint64_t *bitmap; 
};

void buddy_init();
uint64_t  buddy_alloc(uint64_t);
void buddy_free(uint64_t);

// 分配 page_cnt 个页的地址空间，返回分配内存的地址。保证分配的内存在虚拟地址和物理地址上都是连续的
uint64_t alloc_pages(uint64_t);
// 相当于 alloc_pages(1);
uint64_t alloc_page();
// 释放从 addr 开始的之前按分配的内存
void free_pages(uint64_t);

