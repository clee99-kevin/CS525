Project Name : Storage Manager
-Chenen Lee(A20435685)
-Jingeun Jung(A20437470)
-Zhongqiu Peng(A20427765)


Run:
1. Open terminal at project root

2. Type make launch to create a bin file: test_assign1 

3. Type ./test_assign1

4. Type make clean to clean




Functions:
1. initStorageManager() : Initiates the storage manager



Manipulating page files:


2. createPageFile (char *fileName)
        Overwrite the file is existed , or if the file does not exist, the file stream is opened in write mode
        and a new page is allocated.


3. openPageFile (char *fileName, SM_FileHandle *fHandle)
        Updated with the information about the opened file. 
        Return RC_OK if succeeded else return RC_FILE_NOTFOUND and closed it.


4. closePageFile (SM_FileHandle *fHandle)
        Closed the file by setting it to null and return RC_OK.


5. destroyPageFile (char *fileName)
        Checked whether fileName is there. If it is, return RC_FILE_NOT_FOUND else remove(fileName) and return RC_OK



Reading blocks from disk:


6. readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
        Firstly, we should ensure that the pageNum is valid, that means the number should be in range of 0 and the number of all the pages.
        If page number isn't valid then RC_READ_NON_EXISTING_PAGE is returned.
        RC_FILE_NOT_FOUND will also be returned in other cases like one of Database, fHandle or memPage is not valid.
        As long as the page number is valid then the block can be read.
        Position found, pointer set here and the block is read, current page position updated after the read. RC_OK returned.


7. getBlockPos (SM_FileHandle *fHandle)
        firstly, check if fHandle and the file are valid.
        return RC_FILE_NOT_FOUND if one of them is void.
        return the current page position they are both valid.


8. readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
        Set pageNum=0 and implements readBlock(), specified file and page handle.


9. readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
        The number of the position of the block to be read is 1 smaller than the number of the current position.
        The function readBlock() is implemented from newpos (the new block position), and the specified file and page handle.


10. readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
        The position of the current position is set to curPagePos.
        The function readBlock() is implemented from the new block position (curpos), and the specified file and page handle.


11. readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
        The number of the position of the new block is 1 bigger than the number of the current position.
        The function readBlock() is implemented from the new block position (nextpos), and the specified file and page handle.


12. readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
        The position of the last block is set to the value of total pages.
        The function readBlock() is implemented from the new block position (lastpos), and the specified file and page handle.


Writing blocks:


13. writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
	We check see if the page number is valid. 
	Page number should be greater than 0 and less than total number of pages.
	If any of the above are null RC_FILE_NOT_FOUND is returned.
	See if the pointer to the page file is available
	Using the valid file pointer we explore to the given location using fseek()
	If fseek() is successful
 	 - Using fwrite() function, we Write the data to the appropriate location
	 - Store into the memPage passed in the paramter.

	
14. writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
	Write a page to disk as using the current position
	call writeBlock() function with pageNum = current page position as the paramter.


	
15. appendEmptyBlock (SM_FileHandle *fHandle)
	Create an empty block(size = Page_Size)
	Move the cursor(pointer) of the file stream to the last page.
	Write Empty block data and update Total number of empty blocks.(The new last page filled with zero bytes.)
        Total number of pages and current position is updated.
	If an empty block will not be appended(failed) RC_FILE_NOT_FOUNDED is returned.
	

16. ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
	If Database is NULL, RC_FILE_NOT_FOUND is returned.
	Check required number of pages is whether greater than the total number of pages.
	If totalNumPages < numberOfPages, Empty blocks are added using appendEmptyBlock function
