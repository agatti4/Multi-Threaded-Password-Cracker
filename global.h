#ifndef GLOBAL_H
#define GLOBAL_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

extern char* password;
extern char** globalBuffer;
extern FILE* inFileDict;
extern char* outputFile;
extern int start;
extern int end;
extern int num;
extern int passFound;
extern int prodAlive;
extern pthread_mutex_t buffer_mutex;
extern pthread_mutex_t inFile_mutex;
extern pthread_cond_t c_prod;
extern pthread_cond_t c_cons;

#endif 
