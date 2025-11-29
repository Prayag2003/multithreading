#include <iostream>
#include <mutex>
#include <thread>
#include <math.h>
#include <semaphore>
using namespace std;

#define size 5
int buffer[size];

std::binary_semaphore
    signal_to_producer{1},
    signal_to_consumer{0};

void produce()
{
    while (1)
    {
        // Acquire => --
        signal_to_producer.acquire();
        cout << "Produced: ";

        for (int i = 0; i < size; i++)
        {
            buffer[i] = pow((i + 1), 3);
            cout << buffer[i] << " " << flush;
            this_thread::sleep_for(100ms);
        }

        cout << "\n";
        // Release => ++
        signal_to_consumer.release();
    }
}

void consume()
{
    while (1)
    {
        signal_to_consumer.acquire();
        cout << "Consumed: ";
        for (int i = size - 1; i >= 0; i--)
        {
            cout << buffer[i] << " " << flush;
            buffer[i] = 0;
            this_thread::sleep_for(100ms);
        }
        cout << "\n\n";
        signal_to_producer.release();
    }
}

int main()
{
    thread producer(produce);
    thread consumer(consume);

    if (producer.joinable())
        producer.join();

    if (consumer.joinable())
        consumer.join();
}