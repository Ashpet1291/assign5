
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>



long int getFileSize(char fileName[]) {

	long int fileSize;
	
	// opening the file in read mode 
	FILE* fp = fopen(fileName, "r");
	 
	 // checking if the file exist or not 
    if (fp == NULL) { 
       fprintf(stderr,"Could not open file %s", fileName);
       exit(1); 
    } 
    
    fseek(fp, 0L, SEEK_END); 
    fileSize = ftell(fp); 
    
    fclose(fp); 

	return fileSize;	
}


int main(int argc, char *argv[]) 
{
	long int result;
	
	result = 0;
	
	
	result = getFileSize(argv[1]);
	
	printf("%li", result);
}
