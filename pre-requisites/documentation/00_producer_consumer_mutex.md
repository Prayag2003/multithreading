# Producer-Consumer with Mutex 🔄

## Overview

Classic pattern: one thread produces data, another consumes it.

## Simple Example

```cpp
#include <thread>
#include <mutex>
#include <queue>
#include <iostream>

std::queue<int> buffer;
std::mutex mtx;

void producer() {
    for (int i = 1; i <= 5; i++) {
        mtx.lock();
        buffer.push(i);
        std::cout << "Produced: " << i << "\n";
        mtx.unlock();
    }
}

void consumer() {
    for (int i = 0; i < 5; i++) {
        while (true) {
            mtx.lock();
            if (!buffer.empty()) {
                int value = buffer.front();
                buffer.pop();
                std::cout << "Consumed: " << value << "\n";
                mtx.unlock();
                break;
            }
            mtx.unlock();
        }
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}
```

## Better Version with Lock Guard

```cpp
#include <thread>
#include <mutex>
#include <queue>

std::queue<int> buffer;
std::mutex mtx;

void producer() {
    for (int i = 1; i <= 5; i++) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push(i);
            std::cout << "Produced: " << i << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer() {
    int consumed = 0;
    while (consumed < 5) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!buffer.empty()) {
                int value = buffer.front();
                buffer.pop();
                std::cout << "Consumed: " << value << "\n";
                consumed++;
            }
        }
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}
```

## Issues with This Approach

❌ Consumer busy-waits (wastes CPU)  
❌ No coordination between producer/consumer  
❌ Inefficient sleep delays

**Solution**: Use condition variables (see `09_conditional_variable.cpp`)

## Key Pattern

```
Producer:
1. Lock
2. Add data
3. Unlock

Consumer:
1. Lock
2. Check if data available
3. Get data
4. Unlock
```

## Takeaways

✅ Mutex protects shared queue  
✅ Lock guards ensure unlock  
✅ Works but not optimal  
✅ Consider condition variables for better coordination
