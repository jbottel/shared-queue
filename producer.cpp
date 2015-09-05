#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

#include "SharedQueue.hpp"

const int MAX_QUEUE_SIZE            =   16;
const int HIGHEST_NUMBER_TO_PRODUCE =  100;

void printUsageInfo(char* program_name) {
    std::cerr << "Usage: " << program_name << " share/create time_to_wait_1 time_to_wait_2 time_to_wait_n "<< std::endl;
    std::cerr << "    share/create: 'create' to create a shared queue, 'share' to use an exisiting shared queue" << std::endl;
    std::cerr << "    time_to_wait: How long to wait (in milliseconds) between producing numbers from the queue" << std::endl;
    std::cerr << "    multiple times to wait will create multiple threads." << std::endl;
}

void produceValuesToMemory(int threadID, SharedQueue* sq_ptr, int waitTime, std::mutex* mutex) {
    while(1) {
        int producedInteger = (rand()% HIGHEST_NUMBER_TO_PRODUCE+1);
        sq_ptr->enqueue(producedInteger);
        mutex->lock();
        std::cout << "Thread " << threadID << ": Produced " << producedInteger << std::endl;
        mutex->unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::string arg1 = argv[1];
        std::vector<int> timesToWait; 
        int numberOfThreads = argc - 2;

        // we start the index at 2 because the first two args are not times to wait.
        for (int i = 2; i < numberOfThreads+2; i++) {
            std::string cur = argv[i];

            try {
                int ms = std::stoi(cur);
                timesToWait.push_back(ms);
            }
            catch(...) {
                printUsageInfo(argv[0]);
                exit(0);
            }
        }

        SharedQueue* sq_ptr;
        if (arg1=="create") {
            SharedQueue sq(MAX_QUEUE_SIZE);
            sq_ptr = &sq;
        }
        else if (arg1=="share") {
            SharedQueue sq;
            sq_ptr = &sq;
        }
        else {
            printUsageInfo(argv[0]);
            exit(0);
        }

        SharedQueue sq = *sq_ptr;

        // initalize the random seed.
        srand(time(NULL));

        // Create threads
        std::cout << "Launching " << numberOfThreads << " threads." << std::endl;
        std::vector<std::thread> threads;
        std::mutex mtx;
        for (int i = 0; i < numberOfThreads; i++) {
            threads.push_back(std::thread(produceValuesToMemory, i+1, &sq,timesToWait.at(i), &mtx));
        }

        // Join threads
        for (auto &t : threads) t.join();


    }
    else printUsageInfo(argv[0]);

}
