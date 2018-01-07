#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "sort_file.h"

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

BF_Block * block;
int record_size = 0;
int max_records = 0;

void SR_WriteRecord(void * data, Record record)
{
    /*write the id*/
    memcpy(data, &record.id, sizeof(record.id));
    /*write the name*/
    data += sizeof(record.id);
    strcpy(data, record.name);
    /*write the surname*/
    data += sizeof(record.name);
    strcpy(data, record.surname);
    /*write the city*/
    data += sizeof(record.surname);
    strcpy(data, record.city);
}

void SR_ReadRecord(void * data, Record * record)
{
    /*read id*/
    memcpy(&record->id, data, sizeof(record->id));

    /*read name*/
    data += sizeof(record->id);
    memcpy(&record->name, data, sizeof(record->name));
    /*read surname*/
    data += sizeof(record->name);
    memcpy(&record->surname, data, sizeof(record->surname));
    /*read surname*/
    data += sizeof(record->surname);
    memcpy(&record->city, data, sizeof(record->city));
}

void SR_PrintRecords(Record record)	{
	printf("|ID: %8d|, |Name: %15s|, |Surname: %15s|, |City: %20s| \n",
     record.id, record.name, record.surname, record.city);
}

SR_ErrorCode SR_Init() {
  // Your code goes here
  BF_Block_Init(&block);

  Record record;
  record_size = sizeof(record.id)+sizeof(record.name)+sizeof(record.surname)+sizeof(record.city);
  max_records = (BF_BLOCK_SIZE - sizeof(int))/record_size;

  return SR_OK;
}

SR_ErrorCode SR_CreateFile(const char *fileName) {
  // Your code goes here

  int fd;

  CALL_OR_DIE(BF_CreateFile(fileName));
  CALL_OR_DIE(BF_OpenFile(fileName, &fd));
  CALL_OR_DIE(BF_AllocateBlock(fd, block));


  char * data = BF_Block_GetData(block);

  /*write the metadata*/
  memcpy(data, "This is a sort file", 20);
  /*changes were made so we make it dirty before the unpin*/
  BF_Block_SetDirty(block);
  CALL_OR_DIE(BF_UnpinBlock(block));
  /*close the file descriptor*/
  CALL_OR_DIE(BF_CloseFile(fd));

  return SR_OK;
}

SR_ErrorCode SR_OpenFile(const char *fileName, int *fileDesc) {
  // Your code goes here

  // write your code here
  /*Open the block file*/
  CALL_OR_DIE(BF_OpenFile(fileName, fileDesc));

  /*Get its block*/
  CALL_OR_DIE(BF_GetBlock(*fileDesc, 0, block));

  /*Get pointer to its data*/
  char * data = BF_Block_GetData(block);

  int result =(memcmp(data, "This is a sort file", 20)==0);
  /*unpin the block*/
  CALL_OR_DIE(BF_UnpinBlock(block));

  if(result)
    return SR_OK;
  else
    return SR_ERROR;


}

SR_ErrorCode SR_CloseFile(int fileDesc) {
  // Your code goes here
  // write your code here
  CALL_OR_DIE(BF_CloseFile(fileDesc));
  /*Destroy the block we created in the HP_Init*/
  /*we could create a function named HP_Close but we couldn't
  change the hp_main in order to call it */
  //BF_Block_Destroy(&block);



  return SR_OK;
}

SR_ErrorCode SR_InsertEntry(int fileDesc, Record record) {
  // Your code goes here

  /*We get the block count in order to find the last one*/
  int count;

  CALL_OR_DIE(BF_GetBlockCounter(fileDesc, &count));

  if(record_size <= 0)
    return SR_ERROR;

  char* data;
  if (count > 1)
  {   /* Get the Last Block*/
      CALL_OR_DIE(BF_GetBlock(fileDesc, count-1, block));
      data = BF_Block_GetData(block);
      int record_count;
      memcpy(&record_count, data, sizeof(int));
      /*We check if the block has space for new record*/
      if(record_count < max_records)
      {  /*Go to the first space available*/
          /*update the counter*/
          record_count++;
          memcpy(data, &record_count, sizeof(int));
          /*move our pointer to the position of the next available record*/
          data +=sizeof(int);
          data += ((record_count-1)*record_size);
          /*write the record*/
          SR_WriteRecord(data, record);
          /*changes were made so we make it dirty before the unpin*/
          BF_Block_SetDirty(block);
          CALL_OR_DIE(BF_UnpinBlock(block));

          return SR_OK;
      }
      /*no changes cause its full, so just unpin it*/
      CALL_OR_DIE(BF_UnpinBlock(block));
  }

  /*We need a new Block*/

  /*New Block Allocation*/
  CALL_OR_DIE(BF_AllocateBlock(fileDesc, block));
  data = BF_Block_GetData(block);
  /*write the counter*/
  int one = 1;
  memcpy(data, &one, sizeof(int));
  /*write the record*/
  data += sizeof(int);
  SR_WriteRecord(data,record);
  /*changes were made so we make it dirty before the unpin*/
  BF_Block_SetDirty(block);
  CALL_OR_DIE(BF_UnpinBlock(block));

  return SR_OK;
}

SR_ErrorCode SR_SortedFile(
  const char* input_filename,
  const char* output_filename,
  int fieldNo,
  int bufferSize
) {
  // Your code goes here
  char* data;
  char* tempdata;
  int recs;
  int fd , fd_temp;
  int i, j, jj, temp, swapped=1;
  int input_file_id , copied_file_id , sorted_file_id , copied_blocks;
  char* sorted_file_name = malloc( (strlen(input_filename) +10)*sizeof(char));
  BF_Block *tempblock;
  //copy the original file to the temporary BF_OpenFile

  CALL_OR_DIE(BF_OpenFile(input_filename , &fd));
  CALL_OR_DIE(BF_CreateFile("temp0"));
  printf("OK 0\n" );
  CALL_OR_DIE(BF_OpenFile("temp0" , &fd_temp));
  CALL_OR_DIE(BF_GetBlockCounter(fd,&copied_blocks));
  BF_Block_Init(&tempblock);

  printf("OK 1\n" );
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

  BF_Block_Destroy(&tempblock);
  // printf("KLEINW TO %d\n",fd );
  CALL_OR_DIE(SR_CloseFile(fd));
  fd = fd_temp;
  //
  // printf("OK 2\n" );
  //
  Record* table_recs;
  void* current;
  void* previous;

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
        Record record2;
        SR_ReadRecord(current, &record2);
        SR_PrintRecords(record2);

        previous = data+ (record_size * (j));

        if( Compare(current,previous,fieldNo)==-1 )
        {
					Record record1, record2;

                    SR_ReadRecord(current, &record1);
                    SR_ReadRecord(previous, &record2);

                    SR_WriteRecord(current, record2);
                    SR_WriteRecord(previous, record1);
					swapped=1;//egine allagh
        }
      }
    }

    BF_Block_SetDirty(block);
    CALL_OR_DIE(BF_UnpinBlock(block));
  }

  SR_PrintAllEntries(fd_temp);


  return SR_OK;
}

SR_ErrorCode SR_PrintAllEntries(int fileDesc) {
  // Your code goes here

  int i, j;

  int block_count;
  int record_count;
  Record record;
  char * data;

  CALL_OR_DIE(BF_GetBlockCounter(fileDesc, &block_count));
  /*we ignore the 0 block cause its the one with the metadata*/
  for(i = 1; i < block_count; i++)
  {
      /*get the block*/
      CALL_OR_DIE(BF_GetBlock(fileDesc, i, block));
      data = BF_Block_GetData(block);
      /*get the record count of this block*/
      memcpy(&record_count, data, sizeof(int));
      /*go to 1st record*/
      data += sizeof(int);
      for( j = 0; j < record_count; j++ )
      {
          SR_ReadRecord(data, &record);
          SR_PrintRecords(record);
          /*next record*/
          data +=record_size;
      }
      /*unpin this block as we read all his records*/
      CALL_OR_DIE(BF_UnpinBlock(block));
  }
  return SR_OK;
}
