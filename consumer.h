#ifndef CONSUMER_H
#define CONSUMER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "global.h"
#include "sha-256.h"

void* consumerMain();
void getWord(char*);
void consume(char*);
void createVariants(char*, char**);
void swapChar(char*, char*, char, char);
void testVariants(char**);

#endif 
