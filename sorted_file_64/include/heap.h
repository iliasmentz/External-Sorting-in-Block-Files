#ifndef HEAP_STRUCT_H
#define HEAP_STRUCT_H

#include "sort_file.h"


typedef struct heap_node{
    Record * record;
    int block_pos;
    int block_num;
    int block_records;
}heap_node;

typedef struct heap{
    heap_node * values;
    int level;
    int size;
    int fieldNo;
}heap;

heap * create_heap(int size, int level, int fieldNo);
heap_node * pop_heap(heap * my_heap);
void update_heap(heap * my_heap, heap_node * node);
void add_heap(heap * my_heap, heap_node * node);
void delete_heap(heap ** my_heap);

#endif
