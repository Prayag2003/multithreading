#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

std::atomic<int> counter;
int times = 100000;

void run()
{
    for (int i = 0; i < times; i++)
    {
        counter++;
    }
}

int main()
{
    thread t1(run);
    thread t2(run);

    t1.join();
    t2.join();

    cout << counter << "\n";
}