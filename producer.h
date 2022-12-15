#ifndef PRODUCER_H
#define PRODUCER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "global.h"

void* producerMain();
void produce(int*, char**);
void writeToBuffer(char**, int);

#endif 
