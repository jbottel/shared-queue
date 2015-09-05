#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

#include "SharedQueue.hpp"

void printUsageInfo(char* program_name) {
    std::cerr << "Usage: " << program_name << " time_to_wait_1 time_to_wait_2 time_to_wait_n"<< std::endl;
    std::cerr << "    time_to_wait: How long to wait (in milliseconds) between consuming numbers from the queue" << std::endl;
    std::cerr << "    multiple times to wait will create multiple threads" << std::endl;
}

void consumeValuesFromMemory(int threadID, SharedQueue* sq_ptr, int waitTime, std::mutex* mutex) {
    while(1) {
        mutex->lock();
        std::cout << "Thread " << threadID << ": Consumed " << sq_ptr->dequeue() << std::endl;
        mutex->unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string arg1 = argv[1];
        std::vector<int> timesToWait;
        int numberOfThreads = argc - 1;
        for (int i = 1; i < numberOfThreads+1; i++) {
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
        SharedQueue sq;

        // Create threads
        std::cout << "Launching " << numberOfThreads << " threads." << std::endl;
        std::vector<std::thread> threads;
        std::mutex mtx;
        for (int i = 0; i < numberOfThreads; i++) {
            threads.push_back(std::thread(consumeValuesFromMemory, i+1, &sq, timesToWait.at(i), &mtx));
        }

        // Join threads
        for (auto &t : threads) t.join();

    }
    else printUsageInfo(argv[0]);

}
