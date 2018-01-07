#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "heap.h"

int compare(Record* record1,Record* record2,int fieldNo){// girnaei 0 gia isothta,-1 an record1<record2,1 an record1>record2
    if(fieldNo==F_ID){
        if(record1->id==record2->id){
            return 0;
        }else if(record1->id<record2->id){
            return -1;
        }else{
            return 1;
        }
    }else if(fieldNo==F_NAME){
        if(!strcmp(record1->name,record2->name)){
            return 0;
        }else if(strcmp(record1->name,record2->name)<0){
            return -1;
        }else{
            return 1;
        }
    }else if(fieldNo==F_SURNAME){
        if(!strcmp(record1->surname,record2->surname)){
            return 0;
        }else if(strcmp(record1->surname,record2->surname)<0){
            return -1;
        }else{
            return 1;
        }
    }else if(fieldNo==F_CITY){
        if(!strcmp(record1->city,record2->city)){
            return 0;
        }else if(strcmp(record1->city,record2->city)<0){
            return -1;
        }else{
            return 1;
        }
    }
    printf("ERROR to compare the records\n");
    return -2;
}

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
    return &my_heap->values[0];
}

void update_heap(heap * my_heap, heap_node * node)
{
    if (node.record == NULL) {
        my_heap->size--;
        my_heap->values, &my_heap->values[1], (my_heap->size)*sizeof(heap_node));
        return;
    }
    int i;
    for ( i = 1; i < my_heap->size; i++)
    {
        if (compare(my_heap->values[i].record, node->record, my_heap->fieldNo)==1) {
            break;
        }
    }
    memmove(my_heap->values, &my_heap->values[1], (my_heap->size -(my_heap->size - i -1))*sizeof(heap_node));
    my_heap->values[i-1].record = node->record;
    my_heap->values[i-1].block_pos = node->block_pos;
    my_heap->values[i-1].block_num = node->block_num;
    my_heap->values[i-1].block_records = node->block_records;
}

void add_heap(heap * my_heap, heap_node * node)
{

    int i;
    for ( i =0; i < my_heap->size; i++)
    {
        if (compare(my_heap->values[i].record, node->record, my_heap->fieldNo)==1) {
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
