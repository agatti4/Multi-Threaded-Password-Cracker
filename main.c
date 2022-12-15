//StewartGatti_PrgAsst_06.c
//Name: Jeffrey Stewart and Anthony Gatti
//Description: This program implements a password cracker that uses one of the 
//             main attacks against passwords, a dictionary attack. Imagine we
//             obtained access to a hashed password that we wanted to crack, we 
//             could go through a list of possible passwords and hash them to
//             see if they match. To do this efficiently we are using producers
//             and consumers on multiple threads. The producers fill a buffer
//             with words from the dictionary and the consumers try all 
//             variations of a word in the buffer. This continues until the
//             password is cracked or all words in the dictionary have been
//             exhausted and variations tested so no password is found.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include "producer.h"
#include "consumer.h"
#include "global.h"

void initializeVars();

void verifyCommandArg(char** argv, int argc);

void freeMem(pthread_t* threadID);

char* password;
char** globalBuffer;
FILE* inFileDict;
char* outputFile;
int start;
int end;
int num;
int passFound;
int prodAlive;
pthread_mutex_t buffer_mutex;
pthread_mutex_t inFile_mutex;
pthread_cond_t c_prod;
pthread_cond_t c_cons;

int main(int argc, char* argv[]){
    pthread_t* threadID;
    int threadIDSize;
    int i;

    initializeVars(); //Initialize globals and mallocs

    inFileDict = fopen(argv[1], "r"); // Open Dictionary File

    // Verify command line arguments
	verifyCommandArg(argv, argc);

    threadIDSize = atoi(argv[4])+atoi(argv[5]);
    threadID = (pthread_t*) malloc(threadIDSize * sizeof(pthread_t));

    // Prepare Arguments For Threads
    outputFile = argv[3];
    prodAlive = atoi(argv[4]);

    // Create Producer Threads
    for(i=0;i<atoi(argv[4]);i++){
		pthread_create(&threadID[i],NULL,producerMain,NULL);
	}

    // Create Consumer Threads
    for(i=atoi(argv[4]);i<(atoi(argv[4])+atoi(argv[5]));i++){
		pthread_create(&threadID[i],NULL,consumerMain,NULL);
	}

    // Join producer threads
	for(i=0;i<atoi(argv[4]);i++){
		pthread_join(threadID[i],NULL);
	}
    prodAlive = 0; //Set to 0 to show producers are dead
    pthread_cond_broadcast(&c_cons); // Let running consumers know

    //Join consumer threads
	for(i=atoi(argv[4]);i<(atoi(argv[4])+atoi(argv[5]));i++){
		pthread_join(threadID[i],NULL);
	}

    freeMem(threadID); // Free, destroy, and close

	return 0;
}

/**
 * Verifies that the command line arguments entered by the user are valid and
 * that there are enough of them. Also reads password from input file and stores
 * in global variable password. Plus checks if dictionary file is valid.
 * @param argv: array containing the command line arguments input by the user
 * @param argc: The amount of command line arguments
 * @returns nothing
 * @throws exit(1) error if there are not enough arguments or if any are invalid
 */
void verifyCommandArg(char** argv, int argc){
    FILE* inFilePass;

	//verify amount of arguments is appropriate
	if(argc < 6){
		printf("%s", "Not enough command line arguments\n");
		exit(1);
	}
    //verify number of producer threads is appropriate
    if(atoi(argv[4]) <= 0){
		printf("%s", "number of producer threads must be greater than 0\n");
		exit(1);
	}
    //verify number of consumer threads is appropriate
    if(atoi(argv[5]) <= 0){
		printf("%s", "number of consumer threads must be greater than 0\n");
		exit(1);
	}

    inFilePass = fopen(argv[2], "r"); //Open password file

    //verify password file is valid
	if(inFilePass == NULL){
		printf("%s", "Error opening password file.\n");
		exit(1);
	}

    //Pull password from file and store in global variable password
    fscanf(inFilePass, "%s", password);

    fclose(inFilePass); // Close password file

    //verify dictionary file is valid
    if(inFileDict == NULL){
		printf("%s", "Error opening dictionary file.\n");
		exit(1);
	}
}

void freeMem(pthread_t* threadID) {
    int i;

    // Deallocate the space created for globalBuffer
    for (i = 0; i < 10000; i++) {
        free(globalBuffer[i]);
    }

    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&inFile_mutex);
    pthread_cond_destroy(&c_prod);
    pthread_cond_destroy(&c_cons);

    free(globalBuffer);
    free(password);
    free(threadID);
    fclose(inFileDict);
}

void initializeVars() {
    int i;

    start = 0;
    end = 0;
    num = 0;
    passFound = 0;
    password = (char*) malloc(65); // Allocate memory for password
    globalBuffer = (char**) malloc(10000 * sizeof(char*));

    for(i = 0; i < 10000; i++){
        globalBuffer[i] = (char*) malloc(45);
    }

    pthread_mutex_init(&buffer_mutex, NULL);
    pthread_mutex_init(&inFile_mutex, NULL);
    pthread_cond_init(&c_prod,NULL);
    pthread_cond_init(&c_cons,NULL);
}