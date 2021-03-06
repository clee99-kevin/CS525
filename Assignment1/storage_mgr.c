#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "storage_mgr.h"
#include "dberror.h"

FILE *Database;

extern void initStorageManager() { 
    printf("Initialized...\n\n");
    Database = NULL;
}


extern RC createPageFile (char *fileName) {
    Database = fopen(fileName, "r");  
    if(Database != NULL) {
        printf("Failed to write...\n\n");
        fclose(Database);
    }
    Database = fopen(fileName, "w+"); //Creates an empty file for writing.
    /*
        *The initial file size should be one page.
        *This method should fill this single page with '\0' bytes.
        */
    //Writing empty page to file
    char *newPage = (char *)malloc(PAGE_SIZE * sizeof(char)); // allocate the newPage into Database
    fwrite(newPage, sizeof(char), PAGE_SIZE, Database); // allocate the position
    printf("Successed to write...\n\n");
    fclose(Database);
    free(newPage);
    return RC_OK;
}


extern RC openPageFile (char *fileName, SM_FileHandle *fHandle) {
    Database = fopen(fileName, "r"); // Opening file stream in read mode.
    if(Database != NULL) {  // Checking Whether file was successfully opened.
        printf("File opened...\n\n");
        int fileSize = ftell(Database);
        (*fHandle).fileName = fileName; //find the file size
        (*fHandle).curPagePos = fileSize/PAGE_SIZE;
        fseek(Database, 0, SEEK_END); // find the number of pages & move to the end of file
        (*fHandle).mgmtInfo = Database;
        fileSize = ftell(Database);
        (*fHandle).totalNumPages = fileSize % PAGE_SIZE == 0 ? fileSize/PAGE_SIZE : fileSize/PAGE_SIZE + 1;
        fclose(Database);
        return RC_OK;
    }
    return RC_FILE_NOT_FOUND;
}		


RC closePageFile(SM_FileHandle *fHandle) {
    RC res = fclose(Database) == 0? RC_OK: RC_FILE_NOT_FOUND;
    return res;
}


RC destroyPageFile(char *fileName) {
    RC res = remove(fileName) != 0 ? RC_FILE_NOT_FOUND : RC_OK; //remove will delete the file fileName and return 0 if successful
    return res;
}


extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    /*The method reads the pageNumth block from a file
     *Then stores its content in the memory pointed to by the memPage page handle
     */
    //Checking whether the pageNum is void or not
    if((*fHandle).totalNumPages < pageNum ) {
        printf("Failed to read block...\n\n");
        return RC_READ_NON_EXISTING_PAGE;
    }
    Database = fopen((*fHandle).fileName, "r"); //Checking whether the fHandle/memPage is void or not
    if(fHandle == NULL || Database == NULL || memPage == NULL) {
        fclose(Database);
        printf("Failed to read block...\n\n");
        return RC_FILE_NOT_FOUND;
    }
    if (fseek((*fHandle).mgmtInfo, pageNum*PAGE_SIZE, SEEK_SET) == 0) {
        fread(memPage, sizeof(char), PAGE_SIZE, (*fHandle).mgmtInfo); //requested block is read
    } else {
        return RC_READ_NON_EXISTING_PAGE;
    }
    (*fHandle).curPagePos = pageNum; // current page position updated to the page number
    fclose(Database);
    printf("Successed to Read block...\n\n");
    return RC_OK;
}


extern int getBlockPos (SM_FileHandle *fHandle) { 
    if(fHandle == NULL || fopen((*fHandle).fileName, "r") == NULL) {   //Check the fHandle and the file itself is void or not
        printf("Failed to get the block position...\n\n");
        return RC_FILE_NOT_FOUND;
    } 
    printf("The current position is: %d ...\n\n", (*fHandle).curPagePos);
	return ((*fHandle).curPagePos);
}

// in the below functions, the readBlock() function is used to read blocks with the given specifications.
extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    printf("Reading the first block...\n\n");
    return readBlock(0, fHandle, memPage);
    // implements readBlock() with pagenum=0, and the specified file and page handle
}


extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    printf("Reading the previous block...\n\n");
    //readBlock() is implemented from the new block position (newpos),
    //and the specified file and page handle
    return readBlock((*fHandle).curPagePos-1 , fHandle, memPage);    //the position of the block to be read is one less than the current position    
}


extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    printf("Reading the current block...\n\n");
    //readBlock() is implemented from the new block position (curpos),
    //and the specified file and page handle
    return readBlock((*fHandle).curPagePos, fHandle, memPage);     // the position of the current position is set to curPagePos
}


extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    printf("Reading the next block...\n\n");
    //readBlock() is implemented from the new block position (nextpos),
    //and the specified file and page handle
    return readBlock((*fHandle).curPagePos+1, fHandle, memPage);     //the position of the new block is one more than the current position
}


extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    printf("Reading the last block...\n\n");
    //readBlock() is implemented from the new block position (lastpos),
    //and the specified file and page handle
    return readBlock((*fHandle).totalNumPages, fHandle, memPage);    //the position of last block is set to the value of total pages
}


extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Check 'PageNum' is lesser than(or 0) the total number of pages
    if(pageNum > (*fHandle).totalNumPages|| pageNum < 0) {
        printf("Writing the block failed..\n\n");
        return RC_WRITE_FAILED;
    } else { Database = fopen((*fHandle).fileName, "r+"); }
    
    if(fHandle == NULL || Database == NULL || memPage == NULL) { //Check if the values are null in which case block cannot be written in
        printf("Writing the block failed..\n\n");
        fclose(Database);
        return RC_FILE_NOT_FOUND; 
    } else {
        fseek(Database, pageNum*PAGE_SIZE*sizeof(char),SEEK_SET); //writing in the requested block
        fwrite(memPage, sizeof(char), PAGE_SIZE, Database);
        (*fHandle).curPagePos = pageNum;
        fclose(Database);
        printf("Writting page into block...\n\n");
        return RC_OK;
    }
}

//calls the writeBlock() function starting from the current position with the specified file and page handle
extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    printf("Writing the current page into the block...\n\n");
    return writeBlock ((*fHandle).curPagePos, fHandle, memPage);
}


// The function will add an empty block to the end of the block
extern RC appendEmptyBlock (SM_FileHandle *fHandle) {
    SM_PageHandle newPage = (char*)malloc(PAGE_SIZE * sizeof(char));
    int isSeekSuccess = fseek(Database, 0, SEEK_END); //move to end of the file
		
    if (isSeekSuccess == 0) { //write the new empty block into the file
        fwrite(newPage, sizeof(char), PAGE_SIZE, Database); 
        free(newPage);
        (*fHandle).totalNumPages += 1;
        (*fHandle).curPagePos = (*fHandle).totalNumPages; //update the current position
        free(newPage); 			
        printf("Adding an empty block...\n\n");
        return RC_OK;
    } else {
        free(newPage); 		
        return RC_WRITE_FAILED;
    }	  
}


extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {	
    Database = fopen((*fHandle).fileName, "a");
    
    if(Database == NULL)
       return RC_FILE_NOT_FOUND;
		  
    while (numberOfPages > (*fHandle).totalNumPages) appendEmptyBlock(fHandle); //Checking if numberOfPages is greater than totalNumPages.then add empty pages until 'numberOfPages'
    
    fclose(Database); 
    return RC_OK;
}
