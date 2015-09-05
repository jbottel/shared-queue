SHARED_MEM_KEY_ONE=1282
SHARED_MEM_KEY_TWO=1281
SHARED_SEMAPHORE_KEY=1283
COMPILER=g++-4.7
FLAGS=-std=c++11
#FLAGS=-std=c++11 -Wall
CMPL_CMD=$(COMPILER) $(FLAGS)
LNK_FLAGS=-lpthread
LNK_CMD=$(LNK_FLAGS)

all: producer consumer

producer: producer.o SharedQueue.o
	    $(CMPL_CMD) producer.o SharedQueue.o -o producer $(LNK_CMD)

consumer: consumer.o SharedQueue.o
	    $(CMPL_CMD) consumer.o SharedQueue.o -o consumer $(LNK_CMD)

consumer.o: consumer.cpp
	    $(CMPL_CMD) -c consumer.cpp 

producer.o: producer.cpp
	    $(CMPL_CMD) -c producer.cpp 

SharedQueue.o: SharedQueue.cpp SharedQueue.hpp
	    $(CMPL_CMD) -c SharedQueue.cpp 

clean:
	    rm *.o
		rm producer consumer
		ipcrm -M $(SHARED_MEM_KEY_ONE)
		ipcrm -M $(SHARED_MEM_KEY_TWO)
		ipcrm -S $(SHARED_SEMAPHORE_KEY)

cleanipc:
		ipcrm -M $(SHARED_MEM_KEY_ONE)
		ipcrm -M $(SHARED_MEM_KEY_TWO)
		ipcrm -S $(SHARED_SEMAPHORE_KEY)
