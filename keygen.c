// CS 344 Assignment 5: 
// By Ashley Pettibone

// sources and inspirations
// getting random element https://rosettacode.org/wiki/Pick_random_element#C



// compile all
// gcc -o enc_server enc_server.c
// gcc -o enc_client enc_client.c
// gcc -o dec_server dec_server.c
// gcc -o dec_client dec_client.c
// gcc -o keygen keygen.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>




// Generates a random string of uppercase chars or a space, length is given by user
void genRandom(const int len) {
    static const char array[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
    // declare array to hold key
    char *s = (char*)malloc(70000 * sizeof(char));
	int i =0;	
	time_t t;
	srand((unsigned)time(&t));
	
    for (i= 0; i <= len; ++i) {
        s[i] = array[rand()%27];
  	//	s[len] = '\n';
        printf("%c", s[i]);    
    }
    
    s[len] = '\n';

}

int main(int argc, char *argv[]) 
{

	int keyLength = 0;

	// convert string to int
	keyLength = atoi(argv[1]);

	// if arg count isnt one, send error to stderr
	if(argc != 2) {
		 fprintf(stderr, "Need to have 1 value");
	}
	else{
		genRandom(keyLength);
	}
}
