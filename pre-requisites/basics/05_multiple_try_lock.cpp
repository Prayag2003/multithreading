#include <bits/stdc++.h>
#include <thread>
#include <mutex>
using namespace std;

int X = 0, Y = 0;
std::mutex mu1, mu2;

void doSomeWorkForSeconds(int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void incrementXorY(int &XorY, std::mutex &m, const char *desc)
{
    for (int i = 0; i < 5; i++)
    {
        m.lock();
        XorY++;
        cout << desc << XorY << "\n";
        m.unlock();
        // Sleep is very important, otherwise the threads would increment the value so faster, that the consumer could never read it as
        doSomeWorkForSeconds(1);
    }
}

void consumeXorY()
{
    int XplusY = 0;
    int consumeTimes = 5;
    while (true)
    {
        int lockResult = std::try_lock(mu1, mu2);
        if (lockResult == -1)
        {
            if (X != 0 && Y != 0)
            {
                consumeTimes--;
                XplusY += X + Y;
                X = 0;
                Y = 0;
                cout << "X Plus Y " << XplusY << "\n";
            }
            mu1.unlock();
            mu2.unlock();
            if (consumeTimes == 0)
                break;
        }
    }
}

int main()
{
    std::thread t1(incrementXorY, std::ref(X), std::ref(mu1), "X ");
    std::thread t2(incrementXorY, std::ref(Y), std::ref(mu2), "Y ");
    std::thread t3(consumeXorY);

    t1.join();
    t2.join();
    t3.join();
}