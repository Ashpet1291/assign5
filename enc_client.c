// CS 344 Assignment 5: 
// By Ashley Pettibone

// sources and inspirations
// most of the send and recv code along with connecting to sockets is frmo the examples in class


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  
#include <unistd.h>
#include <sys/socket.h> 
#include <netdb.h>      

#define MAXSIZE 80000
#define CHUNK 1024



/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/


// Error function used for reporting issues
void error(const char *message) { 
	perror(message); 
    exit(0); 
} 

// makes sure all information is sent
// this code is from Beejs guide given as a reference form the assignment
int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

// returns file size of a file
long int getFileSize(char fileName[]) {
	// variable to hold the file size
	long int fileSize;
	
	// open the file in read mode 
	FILE* fp = fopen(fileName, "r");
	 
	 // check if the file exist or not 
    if (fp == NULL) { 
       fprintf(stderr,"Could not open file %s", fileName);
       exit(1); 
    } 
    // get the file size
    fseek(fp, 0L, SEEK_END); 
    fileSize = ftell(fp); 
    // close the file
    fclose(fp); 
	// return the file size of the given file
	return fileSize;	
}

// checks for bad chars
void checkChars(char tempList[]) {
	
	int i;	
	int charac = 0;
	
	for(i=0; i<strlen(tempList)-2; i++) {
		
		charac = tempList[i];
			
		if(charac != 32) {
			if (charac < 65) {
				fprintf(stderr,"error: input contains bad characters\n");
				memset(tempList, '\0', sizeof(tempList));
				exit(1); 
			}
			if (charac > 90) {
				fprintf(stderr,"error: input contains bad characters\n");
				memset(tempList, '\0', sizeof(tempList));
				exit(1); 
				}	
		}
	}
}


int main(int argc, char *argv[]) {
	int socketFD, portNumber, charsWritten, charsRead; 
    struct sockaddr_in serverAddress;
    struct hostent* hostInfo;
    char buffer[MAXSIZE];

    // Check usage & args
    if (argc < 3) { 
    	fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
    	exit(0); 
    } 

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0){
    	error("CLIENT: ERROR opening socket");
    }

	// Clear out the address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); 
    // The address should be network capable
    serverAddress.sin_family = AF_INET;
    // Store the port number
    portNumber = atoi(argv[3]);
    serverAddress.sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    hostInfo = gethostbyname("localhost"); 
    if (hostInfo == NULL) { 
    	fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    	exit(0); 
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &serverAddress.sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
	
	
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    	error("CLIENT: ERROR connecting");
    	exit(2);
    }
  
  
    memset(buffer, '\0', MAXSIZE);
    char testString[]="this is enc_client";
    
    // send message to check if enc_client is connected to enc_server
    charsWritten = send(socketFD, testString, sizeof(testString), 0);
    if (charsWritten < 0){
    	error("CLIENT: ERROR writing to socket");
  	}
    
    // get response from server if connected to the right server
    charsRead = recv(socketFD, buffer, sizeof(buffer), 0);
     if(charsRead < 0) {
  		error("CLIENT: ERROR reading from socket");
    }
    
    // check if the test string is in the recieved message
    if (strstr(buffer, testString) != 0) {
        fprintf(stderr,"This is encoding client, error connecting on this port\n");
        exit(2);
    }
    
    long int plainSize = 0;
    
  	// get file size of msg file from client
 	plainSize = getFileSize(argv[1]);
	 	
	char textFileSize[MAXSIZE]; 	
 	//put int in char
 	sprintf(textFileSize, "%d", plainSize); 
 	
    long int keySize = 0; 
	// get file size of key	file
	keySize = getFileSize(argv[2]);	
	
	
	char keySizeString[MAXSIZE]; 
	//put int in char to send	
	sprintf(keySizeString, "%d", keySize);
	
	// check if key size is smaller, if so send error
	if(keySize < plainSize) {
		fprintf(stderr, "ERROR: key size small, can't encrypt \n"); 
		exit(1);
	}
    
   
//    // check plaintextFile file for bad characters
//    checkChars();
	// check key file for bad characters
   // checkChars(argv[2]);
	
	// Write message size to server
    charsWritten = send(socketFD, textFileSize, strlen(textFileSize), 0); 
       	
  	if (charsWritten < 0){
    	error("CLIENT: ERROR writing to socket");
  	}
  	if (charsWritten < strlen(textFileSize)){
    	printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}
  
  
    FILE* plaintextFile;
    // recieve success msg 1
    memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, MAXSIZE, 0);
    if(charsRead < 0) {
  		error("CLIENT: ERROR reading from socket on success");
    }
    
    //send plain text to server
    plaintextFile = fopen(argv[1], "r");  

    // Check if file exists   
    if (plaintextFile == NULL) { 
        fprintf(stderr,"Could not open file %s", argv[1]);
        exit(1); 
    } 
   
    char msg[MAXSIZE];
    memset(msg, '\0', sizeof(msg));
    
    // Get input from the user, trunc to buffer - 1 chars, leaving \0
    fgets(msg, sizeof(msg) - 1, plaintextFile);
    // Remove the trailing \n that fgets adds
    
    msg[strcspn(msg, "\n")] = '\0'; 
	// close the file
    fclose(plaintextFile); 

	// check plaintextFile file for bad characters
    checkChars(msg);    


    int len;
    char variable[] = "@";
	strcat(msg, variable);   
    
	// Send message to server
    len = strlen(msg);
	if (sendall(socketFD, msg, &len) == -1) {
		
		
    perror("sendall");
    printf("We only sent %d bytes because of the error!\n", len);
	} 
  	memset(msg, '\0', sizeof(msg));
  
  
    // receive success 2
	memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, MAXSIZE, 0);
    if(charsRead < 0) {
  		error("CLIENT: ERROR reading from socket 2");
    }
    
    FILE* keyFile;
	// open key file
    keyFile = fopen(argv[2], "r");
    
    // Check if file exists 
    if (keyFile == NULL) { 
        fprintf(stderr,"Could not open file %s", argv[2]);
        exit(1); 
    } 
    memset(buffer, '\0', sizeof(buffer));
    // Get input from the user, trunc to buffer - 1 chars, leaving \0
    fgets(buffer, sizeof(buffer) - 1, keyFile);
    // Remove the trailing \n that fgets adds
 
    fclose(keyFile);   

    
	// Write keysize to server
    charsWritten = send(socketFD, keySizeString, strlen(keySizeString), 0); 
  	if (charsWritten < 0){
    	error("CLIENT: ERROR writing to socket");
  	}
  	if (charsWritten < strlen(keySizeString)){
    	printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}

	char tempBuff[MAXSIZE];
	// recieve succes 3  
    memset(tempBuff, '\0', sizeof(tempBuff));
    charsRead = recv(socketFD, tempBuff, MAXSIZE, 0);
    if(charsRead < 0) {
  		error("CLIENT: ERROR reading from socket 3");
    }
  
  
    // send key string to server
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); 
	    	
    if (charsWritten < 0){
    	error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(buffer)){
    	printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

	// read in ciphertext
	char tempString[MAXSIZE];
  
    memset(buffer, '\0', MAXSIZE);
    memset(tempString, '\0', MAXSIZE);
     
    while (strstr(tempString, "@") == NULL) {
		memset(buffer, '\0', sizeof(buffer));
		charsRead = recv(socketFD, buffer, CHUNK, 0); 
		if (charsRead < 0){
    		error("ERROR reading from socket");
    	} 
		strcat(tempString, buffer);	
	}

	tempString[strlen(tempString)-1] = '\0';		

    // print ciphertext
    printf("%s\n", tempString);
    fflush(stdout);
	
    // Close the socket
    close(socketFD); 
  return 0;
}
