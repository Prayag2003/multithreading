#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
using namespace std;

const int maxBufferSize = 100;
mutex mut;
condition_variable cond;
deque<int> buffer;

void producer(int val)
{
    while (val > 0)
    {
        unique_lock<mutex> locker(mut);
        cond.wait(locker, []
                  { return buffer.size() < maxBufferSize; });
        buffer.push_back(val);
        cout << "Produced: " << val << "\n";
        val--;
        locker.unlock();
        cond.notify_one();
    }
}

void consumer(int totalToConsume)
{
    for (int i = 0; i < totalToConsume; ++i)
    {
        unique_lock<mutex> locker(mut);
        cond.wait(locker, []
                  { return !buffer.empty(); });

        int val = buffer.back();
        buffer.pop_back();
        cout << "Consumed: " << val << "\n";

        locker.unlock();
        cond.notify_one();
    }
}

int main()
{
    int itemsToProduce = 50;
    thread t1(producer, itemsToProduce);
    thread t2(consumer, itemsToProduce);

    t1.join();
    t2.join();

    return 0;
}