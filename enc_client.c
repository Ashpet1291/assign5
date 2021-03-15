// CS 344 Assignment 5: 
// By Ashley Pettibone

// sources and inspirations



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <unistd.h>
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

#define MAXSIZE 80000



/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/


// if enc_client cannot connect to the enc_server server, for any reason (including that it has accidentally tried to connect to the dec_server server), 
// it should report this error to stderr with the attempted port, and set the exit value to 2.

static const char array[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
FILE* plaintextFile;
FILE* keyFile;
char key[MAXSIZE]; 
char textFile[MAXSIZE];
char msg[MAXSIZE];
long int keySize = 0; 
long int plainSize = 0;
char keySizeString[MAXSIZE]; 
char textFileSize[MAXSIZE];

char tempBuff[MAXSIZE];


char cipherText[MAXSIZE];

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// makes sure all information is sent
// this code is from Beejs guide
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
	char charac;
	
	for(i=0; i<strlen(tempList)-2; i++) {
		
		charac = tempList[i];		
	//	printf("this is charac %d\n", charac);
	//	printf("this is char char %s\n", tempList[i]);
		if (charac == ' ') {
		// fine because it's a space	
		}	
		else if((charac >= 'A') && (charac <= 'Z')) {
			// fine because it's uppercase
		}
		else {
			fprintf(stderr,"error: input contains bad characters\n");
			break;
			exit(1);
		}	
	}
}


int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead; 
  struct sockaddr_in serverAddress;
  struct hostent* hostInfo;
  char buffer[MAXSIZE];
  
//  printf("this is enc client arg 0 %s\n", argv[0]);
//  printf("this is enc client arg 1 %s\n", argv[1]);
//  printf("this is enc client arg 2 %s\n", argv[2]);
//  printf("this is enc client arg 3 %s\n", argv[3]);
//  printf("this is arg 4 %s\n", argv[4]);
  
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
  
  
  ///////////////////////////////////////////////////////////////////////////
  //printf("this is client %s\n", argv[3]); first port
  
//  if(portNumber != )
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
	
	
	 //if enc_client cannot connect to the enc_server server, 
	 //for any reason (including that it has accidentally tried to connect to the dec_server server)
	// it should report this error to stderr with the attempted port, 
	 //and set the exit value to 2. Otherwise, upon successfully running and terminating, enc_client should set the exit value to 0.
	
	
  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
    exit(2);
  }
  	// get file size of msg file
 	plainSize = getFileSize(argv[1]);
 	
 	//put int in char
 	sprintf(textFileSize, "%d", plainSize); 
 	
	// get file size of key	file
	keySize = getFileSize(argv[2]);
	
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
	
		// Send message size to server
 	// Write to the server
    charsWritten = send(socketFD, textFileSize, strlen(textFileSize), 0); 
    ////////////////////////////////////////////////////////////////////////////////////
   // printf("client: This is size of sending char txtfilesize %d\n", strlen(textFileSize));
    	
    	
    	
  	if (charsWritten < 0){
    	error("CLIENT: ERROR writing to socket");
  	}
  	if (charsWritten < strlen(textFileSize)){
    	printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}
  
    // recieve success msg 1
    memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, MAXSIZE, 0);
    if(charsRead < 0) {
  		error("CLIENT: ERROR reading from socket");
    }
    
    //send plain text to server
    plaintextFile = fopen(argv[1], "r");  

    // Check if file exists   
    if (plaintextFile == NULL) { 
        fprintf(stderr,"Could not open file %s", argv[1]);
        exit(1); 
    } 
   
    memset(msg, '\0', sizeof(msg));
    // Get input from the user, trunc to buffer - 1 chars, leaving \0
    fgets(msg, sizeof(msg) - 1, plaintextFile);
    // Remove the trailing \n that fgets adds
     
    ////////////////////////////////////////////////////////////////////////////////////
   // printf("Client: This is size of msg being sent after fgets %d\n", strlen(msg));
    
    msg[strcspn(msg, "\n")] = '\0'; 

    fclose(plaintextFile); 


	// check plaintextFile file for bad characters
    checkChars(msg);    

    int len;

    char variable[] = "$";
	strcat(msg, variable);   
    
	// Send message to server
    len = strlen(msg);
	if (sendall(socketFD, msg, &len) == -1) {
		
	////////////////////////////////////////////////////////////////////////////////////
   // printf("Client: This is size of msg being sent in sendall %d\n", len);
		
    perror("sendall");
    printf("We only sent %d bytes because of the error!\n", len);
	} 
  	memset(msg, '\0', sizeof(msg));
  
  
    // receive success 2
	memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, MAXSIZE, 0);
    if(charsRead < 0) {
  		error("CLIENT: ERROR reading from socket");
    }
    
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

    
	// Send keysize to server
 	// Write to the server
    charsWritten = send(socketFD, keySizeString, strlen(keySizeString), 0); 
  	if (charsWritten < 0){
    	error("CLIENT: ERROR writing to socket");
  	}
  	if (charsWritten < strlen(keySizeString)){
    	printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}

	// recieve succes 3  
    memset(tempBuff, '\0', sizeof(tempBuff));
    charsRead = recv(socketFD, tempBuff, MAXSIZE, 0);
    if(charsRead < 0) {
  		error("CLIENT: ERROR reading from socket");
    }
  
  
    // send key string to server
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); 
    
    ////////////////////////////////////////////////////////////////////////////////////
   // printf("CLIENT: This is size of sending key %d\n", strlen(buffer));
    	
    if (charsWritten < 0){
    	error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(buffer)){
    	printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

 
//    // receives encrypted text from server
//    // Clear out the buffer again for reuse
//    memset(buffer, '\0', sizeof(buffer));
//    // Read data from the socket, leaving \0 at end
//    charsRead = recv(socketFD, buffer, MAXSIZE, 0); 
    
    
    
    	memset(buffer, '\0', MAXSIZE);
    	memset(cipherText, '\0', MAXSIZE);


		while (strstr(cipherText, "$") == NULL) {
			memset(buffer, '\0', sizeof(buffer));
			charsRead = recv(connectionSocket, buffer, MAXSIZE, 0); 
			if (charsRead < 0){
      			error("ERROR reading from socket");
    		} 
			strcat(cipherText, buffer);	
		}

	//	int size = strlen(plaintext)-1;
		cipherText[plainSize-1] = '\0';
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////
   // printf("CLIENT: This is size of recieving encrypt %d\n", strlen(buffer));
    
//    if (charsRead < 0){
//    	error("CLIENT: ERROR reading from socket");
//    }
    // print encoded text
    printf("%s\n", cipherText);
    fflush(stdout);
	
    // Close the socket
    close(socketFD); 
  return 0;
}
