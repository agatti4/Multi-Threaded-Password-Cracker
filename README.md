# Multi-Threaded Password-Cracker
Here is my final project I made for Systems and Multiprogramming.
<br>
This password-cracker is multi-threaded and uses a circular queue with producers and consumers.
Producers pull words from a dictionary file and consumers test 80 variations of each word to see if they are the password in the password file

To compile: Use terminal command gcc -pthread -g -o main main.c producer.c consumer.c sha-256.c
To run: Use terminal command ./main dictionaryFile passwordFile outputFile numberOfProducers numberOfConsumers
