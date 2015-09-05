#ifndef SHAREDQUEUE_HPP
#define SHAREDQUEUE_HPP
class SharedQueue {

    static const int   SHARED_KEY             = 1282;
    static const int   SHARED_KEY_SIZE_MEM    = 1281;
    static const int   BUFFER_SIZE            =  128;
    static const int   SEMAPHORE_KEY          = 1283;
    static const short ADD_KEY                =    1;
    static const short WAIT_KEY               =   -1;

    struct queue {
        int readPosition;
        int writePosition;
        int maxItems;
        int itemArray[BUFFER_SIZE];
    };


    public:
    SharedQueue(unsigned int);
    SharedQueue();

    void enqueue(int);
    int dequeue();
    int getMaxItems();
    void setMaxItems(int);

    int getWritePosition();
    int getReadPosition();

    bool isFull();
    bool isEmpty();

    void waitForSemaphore();
    void returnSemaphore();

    protected:
    queue* theQueue;
    int semaphoreID;
};

#endif
