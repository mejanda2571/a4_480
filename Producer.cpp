//
// Created by Vincent Ejanda on 4/13/23.
//

#include "Producer.h"
#include "SharedData.h"
#include "log.h"

#define BitcoinIs 0
#define EthereumIs 1

//Make void producer function for both of them 
void *producer(void *voidptr) {

	//open the file
    auto *sharedData = (SharedData *) voidptr;

	// get the unique request type
	RequestType request;

	// if is bitcoin, set the request to bitcoin
	if (sharedData->isBitcoin) {
		request = Bitcoin;
	}
	// else if is ethereum, set the request to ethereum
	else {
		request = Ethereum;
	}

	// Signal the main that the produce type is set
	sem_post(&sharedData->produceType);

	while (true) {

		// wait on the totalItemsProduced
		sem_wait(&sharedData->totalItemsProduced);

		// TODO: Break out of the loop condition: All items have been produced
		// Have an int in the shared Data that keeps track of the number of items produced

		if(sharedData->producedItems >= sharedData->numRequests){
			break;
		}
	
		// if bitcoin, sleep for the bitcoin producing time
		if (request == Bitcoin) {
			usleep(sharedData->bitProducingTime * 1000);
		}

		// else if ethereum, sleep for the ethereum producing time
		else {
			usleep(sharedData->ethProducingTime * 1000);
		}

		// wait on the bitcoinMutexSemaphore if the request is bitcoin
		if (request == Bitcoin) {
			sem_wait(&sharedData->bitcoinMutexSemaphore);
		}

		// Wait for unused slots
		sem_wait(&sharedData->unusedSlots);

		 //access buffer exclusively
		 sem_wait(&sharedData->queueMutexSemaphore);
		 //add to queue
		 sharedData->broker.push(request);

		 //update the counter for the number of items produced and call log function
		 sharedData->produced[request]++;

		 //increment the number of items in the queue
		 sharedData->producedItems++;

		 // Update the number of items in the queue
		 sharedData->inRequestQueue[request]++;

		 //log inrequest add
		 log_request_added(request, sharedData->produced, sharedData->inRequestQueue);

		 //unlock buffer
		 sem_post(&sharedData->queueMutexSemaphore);

		//up, inform consumer
		sem_post(&sharedData->usedSlots);
	}
	return NULL;
}
