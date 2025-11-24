#include <bits/stdc++.h>
#include <mutex>
#include <thread>
using namespace std;

// Recursive mutex
recursive_mutex recursive_mut, loop_mut;
int criticalVariable = 0;

void recursion(const char *desc, int loop)
{
    if (loop < 0)
        return;

    // Critical Section begins
    recursive_mut.lock();
    cout << desc << " | Value: " << criticalVariable++ << "\n";
    recursion(desc, loop - 1);

    // NOTE: Unlock should be called as many times as lock was done, otherwise no other thread will be able to access that lock
    recursive_mut.unlock();
    cout << "Unlocked by " << desc << "\n";
    // Critical Section ends
}

void loop()
{
    for (int i = 0; i < 5; i++)
    {
        loop_mut.lock();
        cout << "Locked " << i << "\n";
    }
    for (int i = 0; i < 5; i++)
    {
        loop_mut.unlock();
        cout << "Unlocked " << i << "\n";
    }
}

int main()
{
    int counter = 10;
    thread t1(recursion, "Thread 1 ", counter);
    thread t2(recursion, "Thread 2 ", counter);

    if (t1.joinable())
        t1.join();
    if (t2.joinable())
        t2.join();

    loop();

    return 0;
}