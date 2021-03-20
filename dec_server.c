// CS 344 Assignment 5: 
// By Ashley Pettibone

// sources and inspirations
// to get file size
// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c

// used for reference
// https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language

// to run with grading script ./p5testscript RANDOM_PORT1 RANDOM_PORT2 > mytestresults 2>&1



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 80000
#define CHUNK 1024

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
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
 

int main(int argc, char *argv[]){
  int connectionSocket, newConnectionSocket, charsRead, portNumber;
  char buffer[MAXSIZE];
  struct sockaddr_in serverAddress, clientAddress;
  
  
//  size_t NumberOfElements = sizeof(buffer)/sizeof(buffer[0]);
  socklen_t sizeOfClientInfo = sizeof(clientAddress);
	
	int status;
	pid_t pid;
	
	
			
	int i;
	int plaintextInt;
    int keyInt;
	int result;
	
  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
 
  
   // Set up the address struct for the server socket
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  // The address should be network capable
  serverAddress.sin_family = AF_INET;
  // Store the port number
  portNumber = atoi(argv[1]);
  serverAddress.sin_port = htons(portNumber);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

 
  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

	pid = fork();
	
    switch (pid){

			//if error in fork 
			case -1:
			{
				error("ERROR on fork");
			}
			// child process
			case 0:
			{  
			
			 memset(buffer, '\0', MAXSIZE);
		  //receive test message and send response back
          charsRead = recv(connectionSocket, buffer, sizeof(buffer)-1, 0);
          if (charsRead < 0){
      		error("ERROR reading from socket");
    	  }  
          if (strcmp(buffer, "dec_client") != 0) {
          	        	
              //write error back to client
              char response[]  = "error, this is dec_server cant connect to that client";
              send(connectionSocket, response, sizeof(response), 0);
               memset(response, '\0',  sizeof(response));

          } 
          else {
              //write confirmation back to client
              char response[] = "dec_client";
              send(connectionSocket, response, sizeof(response), 0);
              memset(response, '\0',  sizeof(response));
          }   

	int msgSize;
    memset(buffer, '\0', MAXSIZE);
    // Read the client's message from the socket, this should be the msg size
    charsRead = recv(connectionSocket, buffer, MAXSIZE, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
    }

	
	msgSize = atoi(buffer);

	// send success msg 1 to client
    charsRead = send(connectionSocket, 
                    "I am the server, and I got your message", 39, 0); 
     if (charsRead < 0){
      	error("ERROR writing to socket");
	}

	char plaintext[MAXSIZE];
	char tempBuffer[MAXSIZE];
    // Get the message from the client and display it
    memset(tempBuffer, '\0', MAXSIZE);
    memset(plaintext, '\0', MAXSIZE);
    
    
    // recieve all
    while (strstr(plaintext, "$") == NULL) {
		memset(tempBuffer, '\0', sizeof(tempBuffer));
		charsRead = recv(connectionSocket, tempBuffer, CHUNK, 0); 
		if (charsRead < 0){
    		error("ERROR reading from socket");
    	} 
		strcat(plaintext, tempBuffer);	
	}

	//	int size = strlen(plaintext)-1;
		plaintext[msgSize-1] = '\0';	
	
    
    
	//sends success msg 2
	 charsRead = send(connectionSocket, 
                    "I am the server, and I got your message", 39, 0); 
     if (charsRead < 0){
      	error("ERROR writing to socket");
    }
  
    int keySize;
     // Get the key size from the client
    memset(buffer, '\0', MAXSIZE);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, MAXSIZE, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
    }
   
	keySize = atoi(buffer);
//	printf("SERVER: I received this from the client: \"%d\"\n", keySize);
    // Send a Success message 3 back to the client
    charsRead = send(connectionSocket, 
                    "I am the server, and I got your message", 39, 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }

    
    char key[MAXSIZE];
    
     // Get the key from the client and display it
    memset(buffer, '\0', MAXSIZE);
    // Read the key from the socket
    charsRead = recv(connectionSocket, buffer, MAXSIZE, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
    }
    // prints first message from client
//    printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	
	strcat(key, buffer);
 
    
//    	// check for bad chars or unequal sizes
	if(keySize < msgSize) {
		fprintf(stderr, "ERROR: key size small, can't encrypt"); 
		exit(1);
	}
    
     char decText[MAXSIZE];
	 // decrypt plaintext
	 //  converts chars in plaintext to ints 0-26, all uppercase letters and space char
	  for (i=0; i < msgSize-1; i++){
	  	
	  		// if the char in plaintext is a space put 26 in int plainttext-start at 0
			if(plaintext[i] == ' '){
				plaintextInt = 26;
			}
			else{
				// otherwise take char A-ASCII 65 from char in file to get num 0-26
				plaintextInt = plaintext[i] -'A';
			}
			// if the char in key is a space put 26 in int plainttext-start at 0
			if(key[i] == ' '){
				keyInt = 26;
			}
			else{
				// otherwise take char A-ASCII 65 from char in file to get num 0-26
				keyInt = key[i] - 'A';
			}
			// encrypt plaintext- subtract key from plaintext and add 27, then mod 27 to encrypt
			result = (plaintextInt - keyInt + 27) % 27;
			
			// if result is equal to 26, it's a space
			if(result == 26){
				decText[i] = ' ';
			}
			else{
				// decrypted text
				// add char A to get final original uppercase letter
				decText[i] = 'A' + (char)result;
			}
		}
		
	 int len1;

    char variable[] = "$";
	strcat(decText, variable);   
    
	// Send decryption
    len1 = strlen(decText);
	if (sendall(connectionSocket, decText, &len1) == -1) {
		
		
    perror("sendall error writing to socket");
    printf("We only sent %d bytes because of the error!\n", len1);
	} 
  	memset(decText, '\0', sizeof(decText));
  
	
  	}
  	
  	default:
		{
		pid_t parentPid = waitpid(pid, &status, WNOHANG);
		}
	//	close(connectionSocket);
		}
  	close(connectionSocket);
	}
  // Close the listening socket
  close(listenSocket); 
  return 0;
}
