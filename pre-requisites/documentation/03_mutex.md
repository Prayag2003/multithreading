# Mutex: Protecting Shared Data 🔒

## The Problem: Race Conditions

When multiple threads access and modify the same data, chaos ensues:

```cpp
int counter = 0;

void increment() {
    for (int i = 0; i < 1000; i++) {
        counter++;  // Multiple threads modifying simultaneously
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << "\n";
    // Expected: 2000, Actual: Random (1500, 1800, etc.)
}
```

**Why?** The operation `counter++` is NOT atomic — it's actually:

1. Read the current value
2. Increment it
3. Write it back

If two threads do this simultaneously, one thread's result gets overwritten!

## The Solution: Mutex

A **Mutex** (Mutual Exclusion) is a lock that ensures only one thread can access protected code at a time.

```cpp
#include <thread>
#include <mutex>

int counter = 0;
std::mutex counterMutex;

void increment() {
    for (int i = 0; i < 1000; i++) {
        counterMutex.lock();      // Acquire lock
        counter++;                // Safe now!
        counterMutex.unlock();    // Release lock
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << "\n";  // Always 2000!
    return 0;
}
```

## How Mutex Works

```
Thread 1                Thread 2
   │                       │
   ├─► Lock acquired       │
   │   [Critical Section]  │
   │   counter++           │
   │   Lock released ───┐  │
   │                   │  │
   │                   └─►Lock acquired
   │                      [Critical Section]
   │                      counter++
   │                      Lock released
   │
Synchronized! Safe access.
```

## Critical Section

The **critical section** is the code between `lock()` and `unlock()` where only one thread can run at a time.

```cpp
mutex.lock();
// ← Only one thread can be here at a time
// This is the critical section
std::cout << "Safe from race conditions\n";
mutex.unlock();
```

## Basic Mutex Operations

```cpp
std::mutex m;

m.lock();      // Try to acquire lock (blocks if already locked)
m.unlock();    // Release the lock
m.try_lock();  // Try to acquire without blocking (returns bool)
```

## Complete Example

```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>

int sharedCounter = 0;
std::mutex mtx;

void incrementCounter(int threadID) {
    for (int i = 0; i < 5; i++) {
        mtx.lock();  // Acquire lock

        std::cout << "Thread " << threadID << " increments\n";
        sharedCounter++;

        mtx.unlock();  // Release lock
    }
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 3; i++) {
        threads.push_back(std::thread(incrementCounter, i));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter: " << sharedCounter << "\n";  // Always 15
    return 0;
}
```

## Performance Consideration: Keep Critical Section Small

```cpp
// ❌ BAD - Holding lock for too long
mutex.lock();
int result = expensiveCalculation();  // Takes 10 seconds!
sharedData = result;
mutex.unlock();

// ✅ GOOD - Lock only the critical part
int result = expensiveCalculation();  // Do this outside lock
mutex.lock();
sharedData = result;  // Quick operation
mutex.unlock();
```

## Common Mistakes ⚠️

### Mistake 1: Forgetting to Unlock

```cpp
// ❌ DEADLOCK!
mutex.lock();
std::cout << "Working...\n";
// Forgot unlock() - other threads wait forever!

// ✅ SAFE - Use exceptions won't cause deadlock
```

This is why modern C++ uses **lock guards** (see `07_lock_guard.cpp`).

### Mistake 2: Locking Different Variables

```cpp
// ❌ WRONG - Not protecting the data!
int data1 = 0, data2 = 0;
std::mutex m1;

void badLocking() {
    m1.lock();
    data2++;  // Protecting data1, but modifying data2!
    m1.unlock();
}

// ✅ CORRECT - One mutex per shared resource
std::mutex m1, m2;
void goodLocking() {
    m1.lock();
    data1++;
    m1.unlock();

    m2.lock();
    data2++;
    m2.unlock();
}
```

## Thread-Safe Counter Wrapper

```cpp
class ThreadSafeCounter {
    int value = 0;
    mutable std::mutex mtx;  // mutable allows modification in const functions

public:
    void increment() {
        mtx.lock();
        value++;
        mtx.unlock();
    }

    int getValue() const {
        mtx.lock();
        int result = value;
        mtx.unlock();
        return result;
    }
};

int main() {
    ThreadSafeCounter counter;
    counter.increment();
    std::cout << counter.getValue() << "\n";
    return 0;
}
```

## Types of Mutexes

| Type                   | Use Case                | Can Relock? |
| ---------------------- | ----------------------- | ----------- |
| `std::mutex`           | General synchronization | No          |
| `std::recursive_mutex` | Recursive functions     | Yes         |
| `std::timed_mutex`     | With timeout            | No          |
| `std::shared_mutex`    | Readers/writers         | Special     |

## Key Takeaways

✅ Mutex protects shared data from race conditions  
✅ Keep critical sections as small as possible  
✅ Only one thread can hold a mutex at a time  
✅ Always unlock (or use lock guards!)  
✅ Think carefully about what needs protection

## Next Steps

- Learn about **lock guards** (`07_lock_guard.cpp`) for automatic unlock
- Explore **condition variables** (`09_conditional_variable.cpp`) for signaling

---

**Difficulty Level**: ⭐⭐ Intermediate  
**Estimated Time**: 25-35 minutes  
**Prerequisites**: Basic threading, understanding of race conditions
