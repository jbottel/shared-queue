#include <iostream>
#include <chrono>
#include <cstring>
#include <thread>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "SharedQueue.hpp"


SharedQueue::SharedQueue(unsigned int maxItems = BUFFER_SIZE) {
    std::cout << sizeof(struct queue) << std::endl;
    // This constructor is only used the create the queue.
    // Create the shared memory to hold the size
    int sizeSharedMemID = shmget(SHARED_KEY_SIZE_MEM, sizeof(int), 0666 | IPC_CREAT);
    int* queueSizePtr = static_cast<int*>(shmat(sizeSharedMemID, 0,0));
    int queueSize = *queueSizePtr;
    // queue is 3 ints + an int array of maxItems
    queueSize = (3 * sizeof(int)) + (maxItems * sizeof(int));

    // Create the shared memory for the queue
    int sharedMemID = shmget(SHARED_KEY,queueSize, 0666 | IPC_CREAT);
    theQueue = static_cast<queue*>(shmat(sharedMemID, 0,0));
    theQueue->maxItems = maxItems;
    theQueue->writePosition = 0;
    theQueue->readPosition = 0;
    semaphoreID = semget(SEMAPHORE_KEY, 1, 0666);
    semctl(semaphoreID, 0, SETVAL, 1);
}

SharedQueue::SharedQueue() {
    // This constructor is used to attach to the shared queue in memory.

    // Attach to shared memory to get the size of the queue in memory.
    int sizeSharedMemID = shmget(SHARED_KEY_SIZE_MEM, sizeof(int), 0666 | IPC_CREAT);
    if (sizeSharedMemID == -1) { 
        std::cout << "Could not attach to shared memory." << std::endl;
        exit(1);
    }
        
    int* queueSizePtr = static_cast<int*>(shmat(sizeSharedMemID, 0,0));
    int queueSize = *queueSizePtr;

    // Now that we have the size, let us attach to the queue.
    int sharedMemID = shmget(SHARED_KEY,queueSize, 0666 | IPC_CREAT);
    theQueue = static_cast<queue*>(shmat(sharedMemID, 0,0));
    std::cout << "Attached to memory." << std::endl;
    semaphoreID = semget(SEMAPHORE_KEY, 1, 0666);
    semctl(semaphoreID, 0, SETVAL, 1);
}

int SharedQueue::getMaxItems() { return theQueue->maxItems; }
void SharedQueue::setMaxItems(int mi) { theQueue->maxItems = mi; }

int SharedQueue::getWritePosition() { return theQueue->writePosition; }
int SharedQueue::getReadPosition() { return theQueue->readPosition; }

bool SharedQueue::isFull() { return ( theQueue->readPosition == (theQueue->writePosition+ 1) % theQueue->maxItems); }
bool SharedQueue::isEmpty() { return ( theQueue->readPosition == theQueue->writePosition); }

void SharedQueue::enqueue(int item) {
    bool done = false; 
    while (!done) {
        waitForSemaphore();
        if(!isFull()) {
            //write the item to the array and update the position
            theQueue->itemArray[ theQueue->writePosition ] = item;
            theQueue->writePosition = (theQueue->writePosition + 1) % theQueue->maxItems;
            done = true;
            returnSemaphore();
        }
        else {
            // Sleep for a bit to help reduce CPU load while waiting for the queue to have space.
            // We should return the semaphore before we wait.
            returnSemaphore();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

int SharedQueue::dequeue() {
    bool done = false;
    int item;
    while (!done) {
        waitForSemaphore();
        if (!isEmpty()) {
            //get the item from the array
            item = theQueue->itemArray[ theQueue->readPosition ];
            // change the position
            theQueue->readPosition = (theQueue->readPosition + 1) % theQueue->maxItems;
            done = true;
            returnSemaphore();
        }
        else {
            // Sleep for a bit to help reduce CPU load while waiting for the queue to have something for us.
            // We should return the semaphore before we wait.
            returnSemaphore();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    return item;
}

void SharedQueue::waitForSemaphore() {
    sembuf buf;
    buf.sem_num = 0; buf.sem_flg = 0; buf.sem_op = WAIT_KEY;
    semop(semaphoreID, &buf, 1);
}

void SharedQueue::returnSemaphore() {
    sembuf buf;
    buf.sem_num = 0; buf.sem_flg = 0; buf.sem_op = ADD_KEY;
    semop(semaphoreID, &buf, 1);
}
