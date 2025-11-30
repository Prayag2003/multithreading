# Producer-Consumer with Mutex 🔄

## Overview

The **producer-consumer** pattern is a classic synchronization problem where:

- **Producer**: Creates/inserts data into a shared buffer
- **Consumer**: Removes/processes data from the shared buffer

This example uses **mutex** and **condition variables** to coordinate between threads safely.

### Standard Definition

The producer-consumer problem (also known as the bounded-buffer problem) involves:

- A shared buffer with limited capacity
- One or more producer threads that add items
- One or more consumer threads that remove items
- Synchronization to prevent buffer overflow/underflow

## Understanding the Code

```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>

const int maxBufferSize = 100;
std::mutex mut;
std::condition_variable cond;
std::deque<int> buffer;

void producer(int val) {
    while (val > 0) {
        std::unique_lock<std::mutex> locker(mut);

        // Wait until buffer has space
        cond.wait(locker, []() {
            return buffer.size() < maxBufferSize;
        });

        buffer.push_back(val);
        std::cout << "Produced: " << val << "\n";
        val--;

        locker.unlock();
        cond.notify_one();  // Notify consumer
    }
}

void consumer(int totalToConsume) {
    for (int i = 0; i < totalToConsume; ++i) {
        std::unique_lock<std::mutex> locker(mut);

        // Wait until buffer has items
        cond.wait(locker, []() {
            return !buffer.empty();
        });

        int val = buffer.back();
        buffer.pop_back();
        std::cout << "Consumed: " << val << "\n";

        locker.unlock();
        cond.notify_one();  // Notify producer
    }
}

int main() {
    int itemsToProduce = 50;
    std::thread t1(producer, itemsToProduce);
    std::thread t2(consumer, itemsToProduce);

    t1.join();
    t2.join();

    return 0;
}
```

## How It Works

### Producer Thread Flow

1. **Acquire lock**: Get exclusive access to buffer
2. **Wait for space**: If buffer is full, wait for consumer to make space
3. **Add item**: Insert new item into buffer
4. **Release lock**: Allow other threads to access
5. **Notify consumer**: Signal that new item is available

### Consumer Thread Flow

1. **Acquire lock**: Get exclusive access to buffer
2. **Wait for items**: If buffer is empty, wait for producer to add items
3. **Remove item**: Take item from buffer
4. **Release lock**: Allow other threads to access
5. **Notify producer**: Signal that space is available

### Execution Timeline

```
Time    Producer              Consumer              Buffer
─────────────────────────────────────────────────────────
T1      lock() ✓              (waiting...)          []
T2      wait() → unlock()     (waiting...)          []
T3      (sleeping...)         lock() ✓              []
T4      (sleeping...)         wait() → unlock()     []
T5      lock() ✓              (sleeping...)         []
T6      push(50)              (sleeping...)         [50]
T7      notify_one()          (wakes up)            [50]
T8      unlock()              lock() ✓              [50]
T9      (waiting...)          pop() → 50            []
T10     (waiting...)          notify_one()          []
T11     (wakes up)            unlock()              []
```

## Key Components

### 1. Mutex (mutual exclusion)

```cpp
std::mutex mut;
```

- Protects the shared buffer from concurrent access
- Ensures only one thread modifies buffer at a time

### 2. Condition Variable

```cpp
std::condition_variable cond;
```

- Allows threads to wait for conditions
- Efficiently signals when conditions change
- Avoids busy-waiting

### 3. Predicate Functions

```cpp
// Producer waits for: buffer has space
cond.wait(locker, []() {
    return buffer.size() < maxBufferSize;
});

// Consumer waits for: buffer has items
cond.wait(locker, []() {
    return !buffer.empty();
});
```

**Why predicates?**

- Prevents spurious wakeups
- Verifies condition is actually met
- More reliable than checking after wakeup

## Why Condition Variables?

### Problem: Busy-Waiting

**Without condition variables:**

```cpp
// ❌ Inefficient
void producer() {
    while (true) {
        mut.lock();
        while (buffer.size() >= maxBufferSize) {
            mut.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            mut.lock();
        }
        buffer.push_back(item);
        mut.unlock();
    }
}
```

**Problems:**

- Wastes CPU checking repeatedly
- Adds unnecessary delay
- Doesn't respond immediately

### Solution: Condition Variables

**With condition variables:**

```cpp
// ✅ Efficient
void producer() {
    std::unique_lock<std::mutex> locker(mut);
    cond.wait(locker, []() {
        return buffer.size() < maxBufferSize;
    });
    buffer.push_back(item);
    locker.unlock();
    cond.notify_one();
}
```

**Benefits:**

- Thread sleeps until condition is met
- Wakes up immediately when notified
- No CPU waste

## Complete Example with Multiple Producers/Consumers

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> buffer;
    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;
    size_t max_size;

public:
    ThreadSafeQueue(size_t max) : max_size(max) {}

    void push(T item) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this]() {
            return buffer.size() < max_size;
        });

        buffer.push(item);
        not_empty.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this]() {
            return !buffer.empty();
        });

        T item = buffer.front();
        buffer.pop();
        not_full.notify_one();
        return item;
    }
};

ThreadSafeQueue<int> queue(10);

void producer(int id, int items) {
    for (int i = 0; i < items; i++) {
        queue.push(id * 100 + i);
        std::cout << "Producer " << id << " produced: "
                  << (id * 100 + i) << "\n";
    }
}

void consumer(int id, int items) {
    for (int i = 0; i < items; i++) {
        int item = queue.pop();
        std::cout << "Consumer " << id << " consumed: "
                  << item << "\n";
    }
}

int main() {
    std::vector<std::thread> threads;

    // Create 2 producers
    for (int i = 0; i < 2; i++) {
        threads.push_back(std::thread(producer, i, 5));
    }

    // Create 2 consumers
    for (int i = 0; i < 2; i++) {
        threads.push_back(std::thread(consumer, i, 5));
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
```

## Important Points

✅ **Always use predicate** - Prevents spurious wakeups  
✅ **Notify after unlocking** - Better performance (optional but recommended)  
✅ **Unique lock required** - Condition variables need unlock capability  
✅ **One condition per condition** - Clear semantics  
✅ **Proper lock scope** - Lock only when needed

## Common Patterns

### Pattern 1: Single Producer, Single Consumer

```cpp
// Simple case - one of each
std::thread producer_thread(producer);
std::thread consumer_thread(consumer);
```

### Pattern 2: Multiple Producers, Multiple Consumers

```cpp
// Multiple threads of each type
std::vector<std::thread> producers;
std::vector<std::thread> consumers;

for (int i = 0; i < num_producers; i++) {
    producers.push_back(std::thread(producer, i));
}

for (int i = 0; i < num_consumers; i++) {
    consumers.push_back(std::thread(consumer, i));
}
```

## Comparison with Other Approaches

### Mutex Only (Busy-Waiting)

```cpp
// ❌ Inefficient
while (buffer.empty()) {
    mut.unlock();
    sleep(10ms);
    mut.lock();
}
```

### Condition Variables

```cpp
// ✅ Efficient
cond.wait(lock, []() { return !buffer.empty(); });
```

### Semaphores (See `03_producer_consumer_semaphore.cpp`)

```cpp
// ✅ Simpler for fixed-size buffers
semaphore.acquire();
// Use buffer
semaphore.release();
```

## Real-World Applications

1. **Message Queues**: Producer sends messages, consumer processes them
2. **Task Queues**: Producer adds tasks, worker threads consume them
3. **Logging**: Producer logs events, consumer writes to file
4. **Streaming**: Producer generates data, consumer processes it
5. **Event Processing**: Producer generates events, consumer handles them

## Best Practices

✅ **Use condition variables** for waiting on conditions  
✅ **Keep predicates simple** - Fast to evaluate  
✅ **Notify after unlock** - Better performance  
✅ **Handle spurious wakeups** - Always use predicates  
✅ **Test with multiple producers/consumers** - Real-world scenarios

## Common Mistakes

### Mistake 1: No Predicate

```cpp
// ❌ Wrong - spurious wakeups
cond.wait(lock);
if (buffer.empty()) {  // May be empty even after wakeup!
    // ...
}
```

### Mistake 2: Wrong Lock Type

```cpp
// ❌ Wrong - lock_guard cannot unlock
std::lock_guard<std::mutex> lg(mut);
cond.wait(lg, []() { return ready; });  // Compile error!
```

### Mistake 3: Not Notifying

```cpp
// ❌ Wrong - consumer never wakes up
buffer.push_back(item);
// Forgot to notify!
```

## Next Steps

- Learn about semaphores: `02_binary_semaphore.cpp`
- See semaphore-based producer-consumer: `03_producer_consumer_semaphore.cpp`
- Learn condition variables in detail: `fundamentals/09_conditional_variable.cpp`
