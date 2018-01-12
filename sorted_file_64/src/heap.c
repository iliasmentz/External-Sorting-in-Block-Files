#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "sorting_tools.h"

heap * create_heap(int size, int level, int fieldNo)
{   /* creates a heap with null records and size 0 */
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
{    /* we pop the smaller record as we need to
    get the smaller one and the heap is sorted */
    return &my_heap->values[0];
}

void update_heap(heap * my_heap, heap_node * node)
{   /* this function is called after the pop and we insert an
       element from the block that we just poped */
    /* at this time our 0 position is empty and we need
       either to fill it either to delete it */
    if (node->record == NULL) {
        /* if we get a null record to update the heap
        we know that means that the block that we just poped
        has not any other records left to be written */
        my_heap->size--;
        memmove(my_heap->values, &my_heap->values[1], (my_heap->size)*sizeof(heap_node));
        return;
    }
    int i;
    for ( i = 1; i < my_heap->size; i++)
    {
        /* find the position of the new node */
        if (Compare(my_heap->values[i].record, node->record, my_heap->fieldNo)==1) {
            break;
        }
    }

    /* move the smaller ones one position left */
    memcpy(&my_heap->values[0], &my_heap->values[1], (i -1)*sizeof(heap_node));

    /* write the values */
    my_heap->values[i-1].record = malloc(sizeof(Record));

    /* write the new record */
    my_heap->values[i-1].record->id = node->record->id;
    strcpy(my_heap->values[i-1].record->name, node->record->name);
    strcpy(my_heap->values[i-1].record->surname, node->record->surname);
    strcpy(my_heap->values[i-1].record->city, node->record->city);

    /* update the position values */
    my_heap->values[i-1].block_pos = node->block_pos;
    my_heap->values[i-1].block_num = node->block_num;
    my_heap->values[i-1].block_records = node->block_records;
}

void add_heap(heap * my_heap, heap_node * node)
{    /* this is used in heap initialization of the heap */
    int i;
    for ( i =0; i < my_heap->size; i++)
    {
        if (Compare(my_heap->values[i].record, node->record, my_heap->fieldNo)==1) {
            break;
        }
    }

    if (my_heap->values[i].record == NULL) {
        /* if it is going to be appended in the end */
        my_heap->values[i].record = malloc(sizeof(Record));

        /* copy the record */
        my_heap->values[i].record->id = node->record->id;
        strcpy(my_heap->values[i].record->name, node->record->name);
        strcpy(my_heap->values[i].record->surname, node->record->surname);
        strcpy(my_heap->values[i].record->city, node->record->city);

        /* write the position values */
        my_heap->values[i].block_pos = node->block_pos;
        my_heap->values[i].block_num = node->block_num;
        my_heap->values[i].block_records = node->block_records;
    }
    else {
        /* if it is going to be appended in the middle */
        /* move the bigger nodes one position right */
        memmove(&my_heap->values[i+1], &my_heap->values[i], (my_heap->size - i)*sizeof(heap_node));

        /* write the values */
        my_heap->values[i].record = malloc(sizeof(Record));

        /* copy the record */
        my_heap->values[i].record->id = node->record->id;
        strcpy(my_heap->values[i].record->name, node->record->name);
        strcpy(my_heap->values[i].record->surname, node->record->surname);
        strcpy(my_heap->values[i].record->city, node->record->city);

        /* write the position values */
        my_heap->values[i].block_pos = node->block_pos;
        my_heap->values[i].block_num = node->block_num;
        my_heap->values[i].block_records = node->block_records;
    }
    /* increase heap size */
    my_heap->size++;
}

int is_empty_heap(heap * my_heap)
{   /* check if we have any records in the heap */
    return (my_heap->size == 0) ;
}

void delete_heap(heap ** my_heap)
{    /* free the allocated memory */
    for (int i = 0; i < (*my_heap)->size; i++) {
        /* code */
        free((*my_heap)->values[i].record);
    }
    free((*my_heap)->values);
    free(*my_heap);
}
