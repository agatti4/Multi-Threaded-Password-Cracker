#include "producer.h"

/**
 * Driver function for producer threads. Makes an internal buffer which pulls
 * 100 words from the input dictionary and if space is available in the global
 * buffer, it adds the 100 words.
 * @returns NULL
 */
void* producerMain() {
    int i;
    char** internalBuffer = (char**) malloc((100)* sizeof(char*));
    int* count = (int*) malloc(sizeof(int));
    
    for(i = 0; i < 100; i++){
        internalBuffer[i] = (char*) malloc(45);
    }

    // While file not empty and password not found
    while(!feof(inFileDict) && passFound == 0){
        produce(count, internalBuffer); // Grab word and put in internal buffer

        pthread_mutex_lock(&buffer_mutex); // Lock mutex
        
        // While buffer full
        while (num > 9900) {
            pthread_cond_wait(&c_prod, &buffer_mutex);
            if(passFound == 1){
                for(i = 0; i < 100; i++){
                    free(internalBuffer[i]);
                }
                free(internalBuffer);
                free(count);
                pthread_mutex_unlock(&buffer_mutex);
                return NULL;
            }
        }
        writeToBuffer(internalBuffer,*count);
        pthread_mutex_unlock(&buffer_mutex);
        pthread_cond_signal(&c_cons);
    }

    for(i = 0; i < 100; i++){
        free(internalBuffer[i]);
    }

    free(internalBuffer);
    free(count);
    fflush(stdout);

    return NULL;
}

/**
 * Takes 100 words from the input dictionary file and stores them in an 
 * internal buffer.
 * @param count: number of words in the internalBuffer
 * @returns nothing
 */
void produce(int* count, char** internalBuffer) {
    int i;
    int tempLength;
    char tempWord[45];

    *count = 0;
    pthread_mutex_lock(&inFile_mutex);
    while(*count < 100 && fscanf(inFileDict, "%s", tempWord)==1){
        strcpy(internalBuffer[*count],tempWord);
        *count = *count + 1;
    }
    pthread_mutex_unlock(&inFile_mutex);
}

/**
 * Takes the internalBuffer and writes everything in it to the globalBuffer.
 * @param internalBuffer: array of words to add to the globalBuffer
 * @param count: number of words in the internalBuffer
 * @returns nothing
 */
void writeToBuffer(char** internalBuffer, int count) {
    int i;

    for(i = 0; i < count; i++){
        strcpy(globalBuffer[end+i],internalBuffer[i]);
    }
    end = (end + count) % 10000;
    num+=count;
}
