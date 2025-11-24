#include <bits/stdc++.h>
#include <thread>
using namespace std;

std::mutex m;

/**
 * MUTEX: Mutual Exclusion (samjhauta)
 * Race condition is a situation where 2 or more threads/ process happened to modify a common data at the same time.
 *
 * If there is a race condition then we have to protect it and protected section is called the Critical Section
 *
 * MUTEX: It is used to avoid race condition.
 * Using lock(), unlock()
 */

int myAmount = 0;

void addMoney()
{
    m.lock();
    myAmount++;
    m.unlock();
}

int main()
{
    std::thread t1(addMoney);
    std::thread t2(addMoney);

    t1.join();
    t2.join();

    cout << myAmount << "\n";
    return 0;
}