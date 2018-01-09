#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "sorting_tools.h"

heap * create_heap(int size, int level, int fieldNo)
{
    heap * my_heap = malloc(sizeof(heap));

    my_heap->values = malloc(size*(sizeof(heap_node)));

    for (size_t i = 0; i < size; i++) {
        /* code */
        my_heap->values[i].record = NULL;
    }
    my_heap->level = level;
    my_heap->size = 0;
    my_heap->fieldNo = fieldNo;

    return my_heap;
}

heap_node * pop_heap(heap * my_heap)
{
    // printf("POP\n" );
    return &my_heap->values[0];
}

void update_heap(heap * my_heap, heap_node * node)
{
    if (node->record == NULL) {
        my_heap->size--;
        memmove(my_heap->values, &my_heap->values[1], (my_heap->size)*sizeof(heap_node));
        return;
    }
    int i;
    for ( i = 1; i < my_heap->size; i++)
    {
        if(node->record==NULL){
            printf("NULL Gia i = %d kai size %d\n", i, my_heap->size);
        }
        if (Compare(my_heap->values[i].record, node->record, my_heap->fieldNo)==1) {
            break;
        }
    }
    // printf("THA TO VALW STIN THESI %d\n", i-1 );
    // printf("KANW UPDATE NODE-> pos %d  block %d Record-> %s %s\n", node->block_pos, node->block_num, node->record->name, node->record->surname);
    // printf("METAKINISI TWN %d KAI SIZE %d\n", (my_heap->size -(my_heap->size - (i -1))), i);
    memcpy(&my_heap->values[0], &my_heap->values[1], (i -1)*sizeof(heap_node));

    my_heap->values[i-1].record = malloc(sizeof(Record));
    my_heap->values[i-1].record->id = node->record->id;
    strcpy(my_heap->values[i-1].record->name, node->record->name);
    strcpy(my_heap->values[i-1].record->surname, node->record->surname);
    strcpy(my_heap->values[i-1].record->city, node->record->city);
    //node->record;
    my_heap->values[i-1].block_pos = node->block_pos;
    // printf("MESA STO HEAP POS %d\n", node->block_pos);
    // exit(1111);
    my_heap->values[i-1].block_num = node->block_num;
    my_heap->values[i-1].block_records = node->block_records;
}

void add_heap(heap * my_heap, heap_node * node)
{

    int i;
    for ( i =0; i < my_heap->size; i++)
    {
        if (Compare(my_heap->values[i].record, node->record, my_heap->fieldNo)==1) {
            break;
        }
    }

    if (my_heap->values[i].record == NULL) {
        my_heap->values[i].record = malloc(sizeof(Record));
        my_heap->values[i].record->id = node->record->id;
        strcpy(my_heap->values[i].record->name, node->record->name);
        strcpy(my_heap->values[i].record->surname, node->record->surname);
        strcpy(my_heap->values[i].record->city, node->record->city);
        my_heap->values[i].block_pos = node->block_pos;
        my_heap->values[i].block_num = node->block_num;
        my_heap->values[i].block_records = node->block_records;
    }
    else{

        memmove(&my_heap->values[i+1], &my_heap->values[i], (my_heap->size - i)*sizeof(heap_node));
        my_heap->values[i].record = malloc(sizeof(Record));
        my_heap->values[i].record->id = node->record->id;
        strcpy(my_heap->values[i].record->name, node->record->name);
        strcpy(my_heap->values[i].record->surname, node->record->surname);
        strcpy(my_heap->values[i].record->city, node->record->city);

        my_heap->values[i].block_pos = node->block_pos;
        my_heap->values[i].block_num = node->block_num;
        my_heap->values[i].block_records = node->block_records;
    }
    my_heap->size++;
}

int is_empty_heap(heap * my_heap)
{
    return (my_heap->size == 0) ;
}

void delete_heap(heap ** my_heap)
{
    for (int i = 0; i < (*my_heap)->size; i++) {
        /* code */
        free((*my_heap)->values[i].record);
    }
    free((*my_heap)->values);
    free(*my_heap);
}
