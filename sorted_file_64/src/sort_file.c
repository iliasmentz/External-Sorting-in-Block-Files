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

BF_Block * block;
int record_size = 0;
int max_records = 0;


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

  CALL_OR_DIE(BF_CreateFile(filename));
  CALL_OR_DIE(BF_OpenFile(filename, &fd));
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
  BF_Block_Destroy(&block);



  return SR_OK;
}

SR_ErrorCode SR_InsertEntry(int fileDesc,	Record record) {
  // Your code goes here

  return SR_OK;
}

SR_ErrorCode SR_SortedFile(
  const char* input_filename,
  const char* output_filename,
  int fieldNo,
  int bufferSize
) {
  // Your code goes here

  return SR_OK;
}

SR_ErrorCode SR_PrintAllEntries(int fileDesc) {
  // Your code goes here

  return SR_OK;
}
