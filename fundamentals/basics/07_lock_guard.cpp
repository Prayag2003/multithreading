#include <bits/stdc++.h>
#include <mutex>
#include <thread>
using namespace std;

/**
 * LOCK GUARD:
 * A very light wrapper for owning mutexes on scoped basis.
 * Acquires the mutex lock the moment we create an object of the lock_guard.
 * Automatically removes the lock when it goes out of scope.
 * we cannot explicity unlock the lock guard and cant create copy of it.
 */

std::mutex mu;
int buffer = 0;

void loop(const char *threadId, int loopFor)
{
    // Lock guard is there for the respective scope
    std::lock_guard<mutex> lg(mu);
    for (int i = 0; i < loopFor; i++)
    {
        buffer++;
        cout << threadId << " " << buffer << "\n";
    }
}

int main()
{
    thread t1(loop, "Thread 1: ", 5);
    thread t2(loop, "Thread 2: ", 5);

    t1.join();
    t2.join();

    return 0;
}