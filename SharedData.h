//
// Created by Vincent Ejanda on 4/13/23.
//
#ifndef A4_SHAREDDATA_H
#define A4_SHAREDDATA_H
#include <semaphore.h>
#include <queue> 
#include <mutex>
#include "cryptoexchange.h"
#include "log.h"
#include "Producer.h"
#include "Consumer.h"
#include <unistd.h>



// All the defaults that are being usedi n the program
//define default no delay
#define DEFAULT_NO_DELAY 0
#define DEFAULT_NUM_REQUESTS 100
#define DEFAULT_NO_DELAY 0
#define MAX_BITCOIN 5
#define MAX_QUEUE_SIZE 16
#define BADFLAG 1
#define MILLISECONDS 1000


//make a struct for the shared data
struct SharedData {
	//Semaphores for all queues	
	sem_t queueMutexSemaphore;
	sem_t bitcoinMutexSemaphore;
	sem_t usedSlots;
	sem_t unusedSlots;
	sem_t lastItem;

	// semaphores for unqiue thread attributes and total number of items produced
	sem_t consumeType;
	sem_t produceType;
	sem_t totalItemsProduced;

	//Parameters for the options in the main
	int numRequests;
	int xConsumeTime;
	int yConsumeTime;
	int bitProducingTime;
	int ethProducingTime;

	//keeps track of the number of items produced
	unsigned int produced[RequestTypeN];
	unsigned int inRequestQueue[RequestTypeN];
	
	//keeps track of the number of items consumes
	unsigned int consumed[ConsumerTypeN][RequestTypeN];
	int consumedItems;
	int producedItems;
	int totalItems;

	//checking if its a bitcoin or not 
	bool isBitcoin;
	bool isBlockChainX;

	//shared buffer for the queue
	std::queue<RequestType> broker;

	

};
#endif //A4_SHAREDDATA_H
