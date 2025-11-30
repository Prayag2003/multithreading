# Producer-Consumer with Semaphore 🔄

## Overview

Use semaphores to coordinate producer and consumer efficiently.

## Complete Example

```cpp
#include <thread>
#include <semaphore>
#include <queue>
#include <iostream>

const int BUFFER_SIZE = 3;
std::queue<int> buffer;
std::binary_semaphore fullSlots(0);    // Start empty
std::binary_semaphore emptySlots(BUFFER_SIZE);  // Start full

void producer() {
    for (int item = 1; item <= 10; item++) {
        emptySlots.acquire();  // Wait if buffer full

        buffer.push(item);
        std::cout << "Produced: " << item << "\n";

        fullSlots.release();   // Signal data available
    }
}

void consumer() {
    for (int i = 0; i < 10; i++) {
        fullSlots.acquire();   // Wait if buffer empty

        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << item << "\n";

        emptySlots.release();  // Signal space available
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

## How It Works

```
Producer                Buffer (Size: 3)           Consumer
   │                         │                        │
acquire(empty)  ┌──────────────────────┐
   │            │ [1] [2] [3] [ ] [ ]  │
   └─ OK        │ (full)               │
produce(1)      │                      │
release(full)   │                      │
   │       notify: "Data ready!"       ├──── acquire(full)
   │                                   ├── OK
   │                                   │ consume(1)
   │                                   │ release(empty)
   │                                   │ "Space available"
   │ acquire(empty) OK                 │
produce(2)      │                      │
```

## Bounded Buffer Pattern

This is the classic **bounded buffer** problem:

- Producer waits if buffer full
- Consumer waits if buffer empty
- Maximum size maintained automatically

## Advantages Over Mutex

✅ More efficient (no busy-waiting)  
✅ Built-in coordination  
✅ Clear producer/consumer roles  
✅ No need for condition variables

## Semaphore Counters

```
emptySlots: Tracks available space
- Starts at BUFFER_SIZE
- Producer acquire() to use space
- Producer release() when done

fullSlots: Tracks available data
- Starts at 0
- Consumer acquire() to get data
- Consumer release() when done
```

## Multiple Producers/Consumers

```cpp
std::thread producers[3];
std::thread consumers[2];

for (int i = 0; i < 3; i++) {
    producers[i] = std::thread(producer);
}
for (int i = 0; i < 2; i++) {
    consumers[i] = std::thread(consumer);
}

for (int i = 0; i < 3; i++) producers[i].join();
for (int i = 0; i < 2; i++) consumers[i].join();
```

## Key Pattern

```
Semaphore Strategy:
- emptySlots: Controls producer (waits if full)
- fullSlots: Controls consumer (waits if empty)
- Balanced: acquire() by one, release() by other
```

## Takeaways

✅ Semaphores elegantly solve producer-consumer  
✅ Bounded buffer maintained automatically  
✅ No condition variables needed  
✅ Cleaner than mutex + conditional_variable  
✅ Works with multiple producers/consumers

## Comparison

```cpp
// With Mutex + CV (verbose)
cv.wait(lock, []() { return !buffer.empty(); });

// With Semaphore (simple)
fullSlots.acquire();
```
