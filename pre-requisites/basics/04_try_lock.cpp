#include <bits/stdc++.h>
#include <thread>
#include <mutex>
using namespace std;

/**
 * Try Lock: Tries to lock the mutex
 * If success, lock is acquired
 * else fails to acquire
 * Thus called as non blocking
 *
 */

int counter = 0;
std::mutex mu;

void increaseCounterTo10000()
{
    for (int i = 0; i < 10000; i++)
    {
        if (mu.try_lock())
        {
            counter++;
            mu.unlock();
        }
    }
}

int main()
{
    std::thread t1(increaseCounterTo10000);
    std::thread t2(increaseCounterTo10000);

    t1.join();
    t2.join();

    cout << "Counter could increase upto: " << counter << "\n";
}