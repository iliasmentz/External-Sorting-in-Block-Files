#ifndef HEAP_STRUCT_H
#define HEAP_STRUCT_H

#include "sort_file.h"

typedef struct heap_node {
    Record * record;
    int block_pos;
    int block_num;
    int block_records;
} heap_node;

typedef struct heap {
    heap_node * values;
    int level;
    int size;
    int fieldNo;
} heap;

heap * create_heap(int size, int level, int fieldNo);
heap_node * pop_heap(heap * my_heap);
void update_heap(heap * my_heap, heap_node * node);
void add_heap(heap * my_heap, heap_node * node);
int is_empty_heap(heap * my_heap);
void delete_heap(heap ** my_heap);

int Compare(Record* record1,Record* record2,int fieldNo);
void Sort_Each_BufferSize_Blocks(int fd_temp , int copied_blocks , int fieldNo, int bufferSize);
void Copy_Block(int fd , int fd_temp);
char * sort_by_level(int file_id, int level, int fieldNo, int bufferSize);


#endif
