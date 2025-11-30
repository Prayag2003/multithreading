#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

mutex m1, m2;

void thread1()
{
    m1.lock();
    m2.lock();
    cout << "Inside thread 1" << "\n";
    m1.unlock();
    m2.unlock();
}

void thread2()
{
    m2.lock();
    m1.lock();
    cout << "Inside thread 2" << "\n";
    m2.unlock();
    m2.lock();
}

int main()
{
    thread t1(thread1);
    thread t2(thread2);
    t1.join();
    t2.join();
}