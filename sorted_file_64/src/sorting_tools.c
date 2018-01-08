#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "sort_file.h"
#include "sorting_tools.h"

#define CALL_OR_DIE(call)     \
{                             \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
}

int Compare(Record* record1,Record* record2,int fieldNo)
{   // girnaei 0 gia isothta,-1 an record1<record2,1 an record1>record2
    if(fieldNo==F_ID){
        if(record1->id==record2->id){
            return 0;
        }else if(record1->id<record2->id){
            return -1;
        }else{
            return 1;
        }
    }else if(fieldNo==F_NAME ){
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

void Sort_Block(int fd_temp , int copied_blocks , int fieldNo)
{
  char* data;
  int recs;
  int fd ;
  int i, j, temp, swapped=1;

  BF_Block * block;
  BF_Block_Init(&block);

  //BF_Block *tempblock;
  Record* table_recs;
  void* current;
  void* previous;
  Record record1, record2;

  //
  for (i=1; i<copied_blocks; i++)
  {
    CALL_OR_DIE(BF_GetBlock(fd_temp, i ,block));
    data = BF_Block_GetData(block);
    memcpy(&recs , data , sizeof(int));
    data += sizeof(int);
    // table_recs = (Record *)data;
    swapped = 1;
    while (swapped)
    {
        // printf("RECS %d\n", recs );
      swapped =0;
      for (j = 1; j<recs; j++)
      {
        current = data+ (record_size * (j));
        previous = data+ (record_size * (j-1));

        SR_ReadRecord(current, &record1);
        SR_ReadRecord(previous, &record2);
        if( Compare(&record1,&record2,fieldNo)==-1 )
        {
                    SR_WriteRecord(current, record2);
                    SR_WriteRecord(previous, record1);
					swapped=1;//egine allagh
        }
      }
    }

    BF_Block_SetDirty(block);
    CALL_OR_DIE(BF_UnpinBlock(block));
  }
  BF_Block_Destroy(&block);

}

void Copy_Block(int fd , int fd_temp)
{
  char* data;
  char* tempdata;
  int i, j, swapped=1;
  BF_Block * block;
  BF_Block_Init(&block);
  BF_Block *tempblock;
  int copied_blocks;
  //copy the original file to the temporary BF_OpenFile

  CALL_OR_DIE(BF_GetBlockCounter(fd,&copied_blocks));
  BF_Block_Init(&tempblock);

  for (i=0; i<copied_blocks; i++)
  {
    CALL_OR_DIE(BF_GetBlock(fd , i , block));
    data = BF_Block_GetData(block);

    CALL_OR_DIE(BF_AllocateBlock(fd_temp,tempblock));
    CALL_OR_DIE(BF_GetBlock(fd_temp , i , tempblock));
    tempdata = BF_Block_GetData(tempblock);

    memcpy(tempdata , data , BF_BLOCK_SIZE);

    BF_Block_SetDirty(tempblock);
    CALL_OR_DIE(BF_UnpinBlock(tempblock));
    CALL_OR_DIE(BF_UnpinBlock(block));

  }

  // SR_PrintAllEntries(fd_temp);
  BF_Block_Destroy(&block);
  BF_Block_Destroy(&tempblock);

}

char * sort_by_level(int file_id, int level, int fieldNo, int bufferSize)
{
    char *new_file=malloc(sizeof(char)*100);
    sprintf(new_file,"temp_%d_sorted",level);

    SR_CreateFile(new_file);

    BF_Block * block;
    BF_Block_Init(&block);
    BF_Block * new_block;
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
    CALL_OR_DIE(BF_GetBlockCounter(file_id, &count));

    int index[bufferSize-1];
    heap * my_heap = create_heap(bufferSize-1, level, fieldNo);

    int i, j, temp;


    heap_node node;
    node.record = malloc(sizeof(Record));
    for ( i = 1; i < count; i+= (bufferSize-1)*level) {


        for (j = 0; j < bufferSize-1; j ++) {
            /* code */
            if(i+(j*level) > count)
                break;

            CALL_OR_DIE(BF_GetBlock(file_id, i+(j*level), block));
            data = BF_Block_GetData(block);

            node.block_num = i;
            node.block_pos = 0;
            memcpy(&node.block_records, data, sizeof(int));

            data += sizeof(int);

            SR_ReadRecord(data, node.record);

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
                // printf("New temp block\n" );
                BF_Block_SetDirty(new_block);
                CALL_OR_DIE(BF_UnpinBlock(new_block));

                CALL_OR_DIE(BF_AllocateBlock(new_file_id, new_block));
                new_data = BF_Block_GetData(new_block);
                new_count =0;
                memcpy(data, &new_count, sizeof(int));
            }
            /*write the record in the new file */
            SR_WriteRecord(new_data+sizeof(int)+(record_size*new_count), *to_be_written->record);
            new_count ++;
            memcpy(new_data, &new_count, sizeof(int));
            free(to_be_written->record);

            /*get next value from the block we just got the value */
            // printf("BLOCK REC %d \n", to_be_written->block_pos);
            if (to_be_written->block_pos == to_be_written->block_records) {
                /* we need the next block from the sorted ones */
                // printf("wra na allaksw block\n" );
                if(to_be_written->block_num%level ==0){
                    /* end of sorted blocks */
                    // printf("Telos block\n" );
                    to_be_written->record = NULL;
                }
                else {
                    /*get the next sorted block */
                    to_be_written->block_num++;
                    to_be_written->block_pos = 0;

                    CALL_OR_DIE(BF_GetBlock(file_id, to_be_written->block_num, block));
                    data = BF_Block_GetData(block);
                    memcpy(&node.block_records, data, sizeof(int));

                    CALL_OR_DIE(BF_UnpinBlock(block));
                }
            }

            if((to_be_written->block_num%level != 0) || (to_be_written->block_pos < to_be_written->block_records)){
                CALL_OR_DIE(BF_GetBlock(file_id, to_be_written->block_num, block));
                data = BF_Block_GetData(block);

                to_be_written->record = malloc(sizeof(Record));
                SR_ReadRecord(data+sizeof(int)+(record_size*to_be_written->block_pos), to_be_written->record);
                to_be_written->block_pos++;
                // printf("AUKSANW Pos\n" );
                CALL_OR_DIE(BF_UnpinBlock(block));
            }

            update_heap(my_heap, to_be_written);

        }
        // perror("ESPASA");
        BF_Block_SetDirty(new_block);
        CALL_OR_DIE(BF_UnpinBlock(new_block));
    }


    BF_Block_Destroy(&block);
    BF_Block_Destroy(&new_block);


}
