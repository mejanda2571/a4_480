//
// Created by Vincent Ejanda on 4/13/23.
//

#include "Consumer.h"
#include "SharedData.h"
#include "log.h"

//Make one void consumer function for both of them 
void *consumer(void *voidptr) {

 	//open the sharedData file
    auto *sharedData = (SharedData *) voidptr;

	//get the unique consumer type
	ConsumerType consumer;

	//if the consumer is a blockChainX, set the consumer to blockChainX
	if (sharedData->isBlockChainX) {
		consumer = BlockchainX;
	}
	//else the consumer is a blockChainY, set the consumer to blockChainY
	else{
		consumer = BlockchainY;
	}

	// Signal the main that the consumer type is set
	sem_post(&sharedData->consumeType);
	
	// Hold the item
	RequestType item;

	//create a bounded buffer in the consumer
	
    while (true) {
		// Have an int in the shared Data that keeps track of the number of items consumed
		//all items have been consumed and use consumeditems
		if (sharedData->consumedItems >= sharedData->numRequests) {
			break;
		}

		// Wait for something there to consume
		sem_wait(&sharedData->usedSlots);

		//mutex signal down
		sem_wait(&sharedData->queueMutexSemaphore);
		
		//item buffer remove
		item = sharedData->broker.front();
		sharedData->broker.pop();

		sharedData->consumed[consumer][item]++;
		
		//increment the number of items consumed
		sharedData->consumedItems++;

		//decrement the number of items in the queue
		sharedData->inRequestQueue[item]--;
		
		// use the log request removed function
		log_request_removed(consumer, item, sharedData->consumed[consumer], sharedData->inRequestQueue);

		//mutex signal up 	
		sem_post(&sharedData->queueMutexSemaphore);

		// Signal there is one less item in the queue
		sem_post(&sharedData->unusedSlots);

		// If the item was bitcoin signal it
		if (item == Bitcoin) {
			sem_post(&sharedData->bitcoinMutexSemaphore);
		}

		// Sleep for the consumer's consume time
		if (consumer == BlockchainX) {

			usleep(sharedData->xConsumeTime * MILLISECONDS);
		}
		else {
			usleep(sharedData->yConsumeTime * MILLISECONDS);
		}
	}
		//uses the precedence constraint
		sem_post(&sharedData->lastItem);

		return nullptr;
}