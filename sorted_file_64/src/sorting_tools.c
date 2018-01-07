#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "sort_file.h"
#include "heap.h"

#define CALL_OR_DIE(call)     \
{                             \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
}

int Compare(Record* record1,Record* record2,int fieldNo){// girnaei 0 gia isothta,-1 an record1<record2,1 an record1>record2
    if(fieldNo==0){
        if(record1->id==record2->id){
            return 0;
        }else if(record1->id<record2->id){
            return -1;
        }else{
            return 1;
        }
    }else if(fieldNo==1){
        if(!strcmp(record1->name,record2->name)){
            return 0;
        }else if(strcmp(record1->name,record2->name)<0){
            return -1;
        }else{
            return 1;
        }
    }else if(fieldNo==2){
        if(!strcmp(record1->surname,record2->surname)){
            return 0;
        }else if(strcmp(record1->surname,record2->surname)<0){
            return -1;
        }else{
            return 1;
        }
    }else if(fieldNo==3){
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

char * sort_by_level(int file_id, int level, int fieldNo, int bufferSize)
{
    char *new_file=malloc(sizeof(char)*100);
    sprintf(new_file,"temp_%d_sorted",blocks_sorted);

    SR_CreateFile(new_file);

    Block * block;
    BF_Block_Init(&block);
    Block * new_block;
    BF_Block_Init(&new_block);

    int new_file_id ;
    CALL_OR_DIE(BF_OpenFile(new_file, &new_file_id));
    CALL_OR_DIE(BF_AllocateBlock(new_file_id, new_block));

    char * data;
    char * new_data;

    new_data = BF_Block_GetData(new_block);
    int new_count = 0;
    memcpy(new_data, &new_count, sizeof(int));


    int count;
    CALL_OR_DIE(BF_GetBlockCounter(fileDesc, &count));

    int index[bufferSize-1];
    heap * my_heap = create_heap(bufferSize-1, level, fieldNo);

    int i, j, temp;


    heap_node node;
    node.record = malloc(sizeof(record));
    for ( i = 0; i < count; i+= (bufferSize-1)*level) {


        for (j = 0; j < bufferSize-1; j ++) {
            /* code */
            if(i+(j*level) > count)
                break;

            CALL_OR_DIE(BF_GetBlock(fileDesc, i+(j*level), block));
            data = BF_Block_GetData(block);

            node.block_num = i;
            node.block_pos = 0;
            memcpy(&node.block_records, data, sizeof(int));

            data += sizeof(int);

            SR_ReadRecord(data, &node.record);

            add_heap(my_heap, &node);
            CALL_OR_DIE(BF_UnpinBlock(block));

        }
        heap_node * to_be_written;
        while(!is_empty_heap(my_heap))
        {
            /* pop the min value */
            to_be_written = pop_heap(my_heap);
            if(new_count == max_records){
                /* our block is full and we need to create a new one */
                BF_Block_SetDirty(new_block);
                CALL_OR_DIE(BF_UnpinBlock(new_block));

                CALL_OR_DIE(BF_AllocateBlock(new_file_id, new_block));
                new_data = BF_Block_GetData(new_block);
                new_count = ++;
                memcpy(data, &new_count, sizeof(int));
            }
            /*write the record in the new file */
            SR_WriteRecord(new_data+sizeof(int)+(record_size*new_count), to_be_written->record)
            new_count ++;
            memcpy(new_data, &new_count, sizeof(int));
            free(to_be_written.record);

            /*get next value from the block we just got the value */
            if (block_pos == block_records) {
                /* we need the next block from the sorted ones */
                if(block_num%level ==0){
                    /* end of sorted blocks */
                    to_be_written.record = NULL;
                }
                else {
                    /*get the next sorted block */
                    to_be_written.block_num++;
                    to_be_written.block_pos = 0;

                    CALL_OR_DIE(BF_GetBlock(fileDesc, to_be_written.block_num, block));
                    data = BF_Block_GetData(block);
                    memcpy(&node.block_records, data, sizeof(int));

                    CALL_OR_DIE(BF_UnpinBlock(block));
                }
            }

            if(block_num%level != 0){
                CALL_OR_DIE(BF_GetBlock(fileDesc, to_be_written.block_num, block));
                data = BF_Block_GetData(block);

                to_be_written.record = malloc(sizeof(Record));
                SR_ReadRecord(data+sizeof(int)+record_size, to_be_written.record);
                CALL_OR_DIE(BF_UnpinBlock(block));
            }

            update_heap(my_heap, to_be_written);

        }
    }


    BF_Block_Destroy(&block);

}
