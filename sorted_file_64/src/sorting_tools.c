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

void Sort_Each_BufferSize_Blocks(int fd_temp , int copied_blocks , int fieldNo, int bufferSize)
{
  // char* data;
  int recs;
  int fd ;
  int i, j, k, temp, swapped=1;

  // BF_Block * block;
  // BF_Block_Init(&block);

  //BF_Block *tempblock;
  Record* table_recs;
  void* current;
  void* previous;
  Record record1, record2;

  //

  int block_index[bufferSize];
  int block_count[bufferSize];
  char * data[bufferSize];
  BF_Block * block[bufferSize];
  int sort_size = bufferSize;

  for( i = 0 ; i < bufferSize ; i++)
  {
    // printf("KANW GIA %d\n", i);
    BF_Block_Init(&block[i]);
  }

  for (i=1; i<copied_blocks; i+=bufferSize)
  {
    if(bufferSize  > copied_blocks -i){
      // printf("EDW %d\n", i );
      sort_size = copied_blocks - i;
    }
    int total_records=0;
    for(j = 0; j < (sort_size); j++){
      // printf("Size %d, sunolo %d\n", i+j, bufferSize);
      CALL_OR_DIE(BF_GetBlock(fd_temp, i+j ,block[j]));
      data[j] = BF_Block_GetData(block[j]);
      memcpy(&block_count[j] , data[j] , sizeof(int));
      total_records += block_count[j];
      data[j] += sizeof(int);
    }
    // table_recs = (Record *)data;
    // swapped = 1;
    // while (swapped)
    // {
    //     // printf("RECS %d\n", recs );
    //   swapped =0;
    //   previous = NULL;
    //   for(j = 0; j < (sort_size); j++)
    //   {
    //     /*code */
    //       for (k = 0; k<block_count[j]; k++)
    //       {
    //         current = data[j]+ (record_size * (k));
    //
    //         if(previous != NULL)
    //         {
    //           SR_ReadRecord(current, &record1);
    //           SR_ReadRecord(previous, &record2);
    //           if( Compare(&record1,&record2,fieldNo)==-1 )
    //           {
    //                       SR_WriteRecord(current, record2);
    //                       SR_WriteRecord(previous, record1);
    //   					          swapped=1;//egine allagh
    //           }
    //         }
    //         previous = current;
    //
    //       }
    //   }
    // }
    quicksort(data, 0, total_records-1, fieldNo);

    for(j = 0; j < (sort_size); j++){
      BF_Block_SetDirty(block[j]);
      CALL_OR_DIE(BF_UnpinBlock(block[j]));
    }
  }

  for(j = 0; j < bufferSize; j++){
    BF_Block_Destroy(&block[j]);
  }

}

int partition(char ** data, int low, int high,  int fieldNo)
{
    char * pivot = data[(high/max_records)] + (record_size * (high%max_records));
    Record piv_record;
    SR_ReadRecord(pivot, &piv_record);

    char * pos_1;
    char * pos_2;
    int i = low - 1;
    Record record1, record2;
    for(int j = low ; j <=high-1; j++)
    {
        pos_1 =data[(j/max_records)] + (record_size * (j%max_records));
        SR_ReadRecord(pos_1, &record1);
        if(Compare(&piv_record,&record1,fieldNo)>=0)
        {
            i++;
            pos_2 = data[(i/max_records)] + (record_size * (j%max_records));
            SR_ReadRecord(pos_2, &record2);

            SR_WriteRecord(pos_1, record2);
            SR_WriteRecord(pos_2, record1);
        }
    }
    pos_2 = data[((i+1)/max_records)] + (record_size * ((i+1)%max_records));
    SR_ReadRecord(pos_2, &record2);

    SR_WriteRecord(pivot, record2);
    SR_WriteRecord(pos_2, piv_record);
    return i+1;

}

void quicksort(char ** data, int low, int high, int fieldNo)
{
        if (low < high)
        {
            int pi = partition(data, low, high, fieldNo);

            quicksort(data, low, pi-1, fieldNo);
            quicksort(data, pi+1, high, fieldNo);
        }
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
    // printf("New file: %s\n", new_file);
    CALL_OR_DIE(BF_OpenFile(new_file, &new_file_id));


    CALL_OR_DIE(BF_AllocateBlock(new_file_id, new_block));

    char * data;
    char * new_data;

    new_data = BF_Block_GetData(new_block);
    int new_count = 0;
    memcpy(new_data, &new_count, sizeof(int));


    int count;
    CALL_OR_DIE(BF_GetBlockCounter(file_id, &count));

    // int index[bufferSize-1];

    int i, j, temp;


    heap_node node;
    node.record = malloc(sizeof(Record));
    for ( i = 1; i < count; i+= (bufferSize-1)*level) {

        heap * my_heap = create_heap(bufferSize-1, level, fieldNo);

        for (j = 0; j < bufferSize-1; j ++) {
            /* code */
            if(i+(j*level) >= count)
                break;
            // printf("for block %d\n", i+(j*level));
            CALL_OR_DIE(BF_GetBlock(file_id, i+(j*level), block));
            data = BF_Block_GetData(block);

            node.block_num = i+(j*level);
            node.block_pos = 1;
            memcpy(&node.block_records, data, sizeof(int));

            data += sizeof(int);

            SR_ReadRecord(data, node.record);
            // printf("NODE-> block_num %d Record-> %s %s\n", node.block_num, node.record->name, node.record->surname);

            add_heap(my_heap, &node);
            CALL_OR_DIE(BF_UnpinBlock(block));

        }

        heap_node * to_be_written;
        heap_node * to_be_stored;

        while(!is_empty_heap(my_heap))
        {
            /* pop the min value */
            to_be_written = pop_heap(my_heap);

            // printf("EKANA POP ME POS %d APO TO BLOCK %d\n", to_be_written->block_pos, to_be_written->block_num);
            if(new_count == max_records){
                /* our block is full and we need to create a new one */
                BF_Block_SetDirty(new_block);
                CALL_OR_DIE(BF_UnpinBlock(new_block));

                int temp;
                CALL_OR_DIE(BF_GetBlockCounter(new_file_id, &temp));

                // printf("New write block %d\n", temp );
                CALL_OR_DIE(BF_AllocateBlock(new_file_id, new_block));
                new_data = BF_Block_GetData(new_block);
                new_count =0;
                memcpy(data, &new_count, sizeof(int));
            }

            if (new_data==NULL) {
                /* code */
                printf("NULL\n" );
                exit(1);
            }
            /*write the record in the new file */
            SR_WriteRecord(new_data+sizeof(int)+(record_size*new_count), *(to_be_written->record));
            new_count ++;
            memcpy(new_data, &new_count, sizeof(int));
            free(to_be_written->record);

            to_be_stored = malloc(sizeof(heap_node));
            to_be_stored->record = NULL;

            to_be_stored->block_num = to_be_written->block_num;
            to_be_stored->block_records = to_be_written->block_records;
            to_be_stored->block_pos = to_be_written->block_pos;
            /*get next value from the block we just got the value */
            // printf("BLOCK REC %d \n", to_be_written->block_num);
            int read_entry=1;
            if (to_be_stored->block_pos >= to_be_stored->block_records) {

                /* we need the next block from the sorted ones */
                // printf("wra na allaksw block\n" );
                if(to_be_stored->block_num%level ==0 || to_be_stored->block_num==count-1){
                    /* end of sorted blocks */
                    // printf("Telos block\n" );
                    read_entry =0;
                    to_be_stored->record = NULL;
                    // to_be_stored->block_num = count;

                }
                else {
                    /*get the next sorted block */
                    // printf("block pou eixa %d\n", to_be_stored->block_num);
                    to_be_stored->block_num ++;

                    // printf("KATHARIZW POS\n" );
                    to_be_stored->block_pos = 0;
                    // printf("1\n" );
                    // printf("block %d\n", to_be_stored->block_num );
                    if (to_be_stored->block_num < count) {
                        /* code */
                        CALL_OR_DIE(BF_GetBlock(file_id, to_be_stored->block_num, block));
                        data = BF_Block_GetData(block);
                        memcpy(&to_be_stored->block_records, data, sizeof(int));

                        CALL_OR_DIE(BF_UnpinBlock(block));
                    }


                }
            }

            if(read_entry){
                CALL_OR_DIE(BF_GetBlock(file_id, to_be_stored->block_num, block));
                data = BF_Block_GetData(block);

                to_be_stored->record = malloc(sizeof(Record));


                // printf("VBAZW NODE-> block_num %d Record-> %s %s\n", to_be_written->block_num, to_be_written->record->name, to_be_written->record->surname);
                // to_be_stored->record = malloc(sizeof(Record));
                // printf("THA PARW TOO STOIXEIO %d gia block %d \n", to_be_stored->block_pos, to_be_stored->block_num );
                SR_ReadRecord(data+sizeof(int)+(record_size*to_be_stored->block_pos), to_be_stored->record);
                // printf("VAZW NODE-> block_num %d Record-> %s %s\n", to_be_stored->block_num, to_be_stored->record->name, to_be_written->record->surname);
                // printf("prin AUKSANW Pos %d\n", to_be_stored->block_pos);

                to_be_stored->block_pos++;
                //                printf("AUKSANW Pos %d\n", to_be_written->block_pos);
                CALL_OR_DIE(BF_UnpinBlock(block));
                //                exit(1);

            }

            // printf("prin csall %d\n", to_be_written->block_pos);
            update_heap(my_heap, to_be_stored);
            if (to_be_stored->record!=NULL) {
                /* code */
                free(to_be_stored->record);
            }
            if (to_be_stored !=NULL) {
                /* code */
                free(to_be_stored);
            }

        }

        delete_heap(&my_heap);
        // perror("ESPASA");
        BF_Block_SetDirty(new_block);
        CALL_OR_DIE(BF_UnpinBlock(new_block));
    }

    free(node.record);

    BF_Block_Destroy(&block);
    BF_Block_Destroy(&new_block);
    CALL_OR_DIE(BF_CloseFile(new_file_id));

    return new_file;

}
