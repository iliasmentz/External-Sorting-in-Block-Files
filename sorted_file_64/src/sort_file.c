#include "sort_file.h"

SR_ErrorCode SR_Init() {
  // Your code goes here

  return SR_OK;
}

SR_ErrorCode SR_CreateFile(const char *fileName) {
  // Your code goes here

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
