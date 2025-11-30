#include <iostream>
#include <thread>
#include <semaphore>
using namespace std;

std::binary_semaphore smpSignalMainToThread{0}, smpSignalThreadToMain{0};

// g++ --std=c++20 02_binary_semaphore.cpp && ./a.out
void ThreadFunc()
{
    /**
     * IMP: Wait for a signal from the main thread by attempting to decrement the semaphore
     */
    smpSignalMainToThread.acquire();

    cout << "[Thread]: Got the signal\n";
    this_thread::sleep_for(3s);

    cout << "[Thread]: Send the signal\n";

    // Notify the main thread
    smpSignalThreadToMain.release();
}

int main()
{
    thread worker(ThreadFunc);
    cout << "[Main]: Send the signal\n";

    /** IMP: signal the worker to start working by incrementing the semaphore count
     *  Release => Increment
     *  Acquire => Decrement
     */
    smpSignalMainToThread.release();

    // wait till the worker is done doing the work by attempting to decrement
    smpSignalThreadToMain.acquire();

    if (worker.joinable())
        worker.join();

    cout << "[Main] Got the signal";
}