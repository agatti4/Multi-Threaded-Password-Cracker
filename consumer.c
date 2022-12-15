#include "consumer.h"

/**
 * Driver function for consumer threads. Takes a word from the global buffer
 * and checks all variations and compares it to the password hash in the given
 * password file
 * @returns NULL
 */
void* consumerMain() {
    char* wordToConsume = (char*) malloc(45);
    int i;

    // While password not found and producers not done
    while(passFound == 0 && prodAlive != 0){
        pthread_mutex_lock(&buffer_mutex);

        // While nothing to consume
        while (num == 0) {
            pthread_cond_wait(&c_cons, &buffer_mutex); // Waits for condition
            if (prodAlive == 0) {
            	free(wordToConsume);
                pthread_mutex_unlock(&buffer_mutex); // Unlock mutex
                return NULL;
            }
        }

        getWord(wordToConsume);
        pthread_mutex_unlock(&buffer_mutex); // Unlock mutex
        pthread_cond_signal(&c_prod); // Signal other producers

        consume(wordToConsume); // Test the word
    }
    
    free(wordToConsume);

    return NULL;
}

/**
 * Pulls a word from the globalBuffer and stores it. Resets the beginning and 
 * end of the globalBuffer.
 * @param wordToConsume: Stores the word from the global buffer
 * @returns nothing
 */
void getWord(char* wordToConsume) { 
    strcpy(wordToConsume, globalBuffer[start]);
    wordToConsume = globalBuffer[start];
    start = (start + 1) % num; // Move position of global buffer
    num--;
}

/**
 * Takes a word and passes it to createVariants to create all the variants and
 * then tests all the variants to see if they are the password.
 * @param wordToConsume: The word from the global buffer to test
 * @returns nothing
 */
void consume(char* wordToConsume) {
    int i;
    char** variants = (char**) malloc(8 * sizeof(char*));
    
    for (i = 0; i < 8; i++) {
        variants[i] = (char*) malloc(strlen(wordToConsume) + 1);
    }

    createVariants(wordToConsume, variants);
    testVariants(variants);

    for (i = 0; i < 8; i++) {
        free(variants[i]);
    }
    free(variants);

}

/**
 * Takes a word and creates an array of 8 variants of the word to be tested.
 * @param wordToConsume: The word from the global buffer to test
 * @param variants: An array to contain the 8 variants to be created
 * @returns nothing
 */
void createVariants(char* wordToConsume, char** variants) {
    int i, j;

    strcpy(variants[0], wordToConsume);

    swapChar(variants[1], wordToConsume, 'i', '!');

    swapChar(variants[2], wordToConsume, 'l', '1');

    swapChar(variants[3], wordToConsume, 'o', '0');

    swapChar(variants[4], variants[1], 'l', '1');

    swapChar(variants[5], variants[1], 'o', '0');

    swapChar(variants[6], variants[3], 'l', '1');

    swapChar(variants[7], variants[4], 'o', '0');

}

/**
 * Swaps the characters in variant to make a specific variant. Swaps
 * orig with swap.
 * @param variant: The final varient
 * @param base: The word from the global buffer to make a variant of
 * @param orig: Original char that is wanted to be swapped
 * @param swap: The new char to replace the original char
 * @returns nothing
 */
void swapChar(char* variant, char* base, char orig, char swap) {
    int i;

    strcpy(variant, base); // Makes a copy of base

    for (i = 0; i < strlen(variant); i++) {
        if (variant[i] == orig) {
            variant[i] = swap;
        }
    }

}

/**
 * Takes an array of 8 variants and tests them each and then tests them by
 * appending 0-9. If the password is found writes to the output file 
 * specified and broadcasts to the producers to wake up, which ends up making
 * them stop due to the password being found
 * @param variants: An array of 8 variants to test
 * @returns nothing
 */
void testVariants(char** variants) {
    int i, j;
    char hash[65]; // Initilize the hash
    char* currentVariant = (char*) malloc((strlen(variants[0]) + 2));
    FILE* outputF;

    // Test each variant
    for (i = 0; i < 8; i++) {
        sha_256_string(hash, variants[i], strlen(variants[i])); 
        if (strcmp(hash, password) == 0) {
            passFound = 1; // Signal password is found
            outputF = fopen(outputFile, "w+"); // Open file
            fprintf(outputF, "%s", variants[i]); // Put in file
            fclose(outputF);
            //Broadcast
            pthread_cond_broadcast(&c_prod);
        }
    }

    // Test each variant with 0-9 appended
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 10; j++) {
            sprintf(currentVariant, "%s%d", variants[i], j);
            sha_256_string(hash, currentVariant, strlen(currentVariant)); 
            if (strcmp(hash, password) == 0) {
                passFound = 1; // Signal password is found
                outputF = fopen(outputFile, "w+"); // Open file
                fprintf(outputF, "%s", currentVariant); // Put in file
                fclose(outputF);
                //Broadcast
                pthread_cond_broadcast(&c_prod);
            }
        }
    }

    free(currentVariant);
}
