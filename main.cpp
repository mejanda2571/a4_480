#include <iostream>
#include <semaphore.h>
#include <queue>
#include <pthread.h>
#include "SharedData.h"
#include "Producer.h"
#include "Consumer.h"
#include "log.h"
#include "unistd.h"
#include <getopt.h>
#include <string>

int main(int argc, char *argv[]) {	
	//Command line switch
    int option;
	//parsing the command line
	int numParse;

	//open shared data
	SharedData sharedData;

	// Set the default values for the arguments
    sharedData.numRequests = DEFAULT_NUM_REQUESTS;
    sharedData.xConsumeTime = DEFAULT_NO_DELAY;
    sharedData.yConsumeTime = DEFAULT_NO_DELAY;
    sharedData.bitProducingTime = DEFAULT_NO_DELAY;
    sharedData.ethProducingTime = DEFAULT_NO_DELAY;

	// parse 5 command line arguments -r,-x, -y, -b, -e
	while ((option = getopt(argc, argv, "r:x:y:b:e:")) != -1) {
		switch (option) {
			//Total number of trade requests to be generated
			case 'r':
				numParse = atoi(optarg);
				if (numParse > 0) {
					sharedData.numRequests = numParse;
				}
				break;
			//Specifies the number of milliseconds N that the consumer using Blockchain X
			case 'x':
			numParse = atoi(optarg);
			if (numParse > 0) {
				sharedData.xConsumeTime = numParse;
			}
			break;
		
			//Specifies the number of milliseconds N that the consumer using Blockchain Y
			case 'y':
			numParse = atoi(optarg);
			if (numParse > 0) {
				sharedData.yConsumeTime = numParse;
			}
			break;
			
		

			//Specifies the number of milliseconds N that the producer for Bitcoin
			case 'b':
				numParse = atoi(optarg);
				if (numParse > 0) {
					sharedData.bitProducingTime = numParse;
				}
			
			break;
		
			//Specifies the number of milliseconds N that the producer for Ethereum
			case 'e':
				numParse = atoi(optarg);
				if (numParse > 0) {
					sharedData.ethProducingTime = numParse;
				}

			break;
		
	}
	}

	//initializing all the ints in the shared data
	sharedData.consumedItems = 0;
	sharedData.producedItems = 0;
	sharedData.totalItems = 0;

	// Init the shared data fields
	sharedData.produced[0] = 0;
	sharedData.produced[1] = 0;

	sharedData.inRequestQueue[0] = 0;
	sharedData.inRequestQueue[1] = 0;

	sharedData.consumed[0][0] = 0;
	sharedData.consumed[0][1] = 0;
	sharedData.consumed[1][0] = 0;
	sharedData.consumed[1][1] = 0;

	// Init the queue
	sharedData.broker = std::queue<RequestType>();

	// 1 is for queue since there is only 1 queue to access, for other things like queue size you want something like 0 or 16 or 5
	sem_init(&sharedData.queueMutexSemaphore, 0, 1);

	sem_init(&sharedData.bitcoinMutexSemaphore, 0, 5);

	sem_init(&sharedData.usedSlots, 0, 0);

	sem_init(&sharedData.unusedSlots, 0, 16);

	sem_init(&sharedData.lastItem, 0, 0);

	sem_init(&sharedData.produceType, 0, 0);

	sem_init(&sharedData.consumeType, 0, 0);

	sem_init(&sharedData.totalItemsProduced, 0, sharedData.numRequests);
	
	//Create a thread for the consumer named blockChainX and blockChainY
	pthread_t consumerBlockChainX, consumerBlockChainY;

	//Creat a thread for bitcoin and ethereum
	pthread_t producerBitcoin, producerEthereum;

	sharedData.isBitcoin = true;
	sharedData.isBlockChainX = true;

	// create a bitcoin thread
	pthread_create(&producerBitcoin, nullptr, &producer , &sharedData);

	// Wait for the produceType semaphore to be signaled
	sem_wait(&sharedData.produceType);

	// Set the isBitcoin flag to false
	sharedData.isBitcoin = false;

	// create a ethereum thread
	pthread_create(&producerEthereum, nullptr, &producer, &sharedData);

	//create blockChainX thread
	pthread_create(&consumerBlockChainX, nullptr, &consumer , &sharedData);

	// Wait for the consumeType semaphore to be signaled
	sem_wait(&sharedData.consumeType);
	
	// Set the isBlockChainX flag to false
	sharedData.isBlockChainX = false;

	//create blockChainY thread
	pthread_create(&consumerBlockChainY, nullptr, &consumer, &sharedData);

	// Wait for the last item to be consumed
	sem_wait(&sharedData.lastItem);

	// Convert arr[2][2] into *arr[2]
	unsigned int *arr[2];
	arr[0] = sharedData.consumed[0];
	arr[1] = sharedData.consumed[1];


	log_production_history(sharedData.produced, arr);
	
}