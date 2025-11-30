#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

std::mutex m1;
int buffer = 0;

void task(const char *threadId, int loopFor)
{
    // Immediately calls the lock on mutex
    std::unique_lock<mutex> lock(m1);

    for (int i = 0; i < loopFor; i++)
    {
        buffer++;
        cout << threadId << " " << buffer << "\n";
    }
}

void lazyLocking(const char *threadId, int loopFor)
{
    // we own this lock, but not dont call lock on mutex immediately
    std::unique_lock<mutex> lock(m1, defer_lock);
    // Do things here
    lock.lock();
    for (int i = 0; i < loopFor; i++)
    {
        buffer++;
        cout << threadId << " " << buffer << "\n";
    }
}

int main()
{
    thread t1(lazyLocking, "T1 ", 10);
    thread t2(lazyLocking, "T2 ", 10);

    t1.join();
    t2.join();
}