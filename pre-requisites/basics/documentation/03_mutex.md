# Mutex: Protecting Shared Data 🔒

## The Problem: Race Conditions

### Definition

A **race condition** is a situation where two or more threads/processes modify common data at the same time, leading to unpredictable and incorrect results.

### Why Race Conditions Occur

Modern CPUs execute operations in multiple steps. Even simple operations like `counter++` are not atomic:

```cpp
// counter++ is actually:
1. Load counter from memory → register
2. Increment register
3. Store register → memory
```

If two threads execute this simultaneously:
```
Thread 1: Load(0) → Increment → Store(1)
Thread 2: Load(0) → Increment → Store(1)  // Lost update!
Result: 1 (should be 2)
```

### Example of Race Condition

```cpp
int counter = 0;

void increment() {
    for (int i = 0; i < 100000; i++) {
        counter++;  // Race condition!
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    
    // Expected: 200000
    // Actual: Random (150000, 180000, etc.)
    cout << counter << "\n";
}
```

## The Solution: Mutex

### Definition

**MUTEX = Mutual Exclusion**

A mutex is a synchronization primitive that ensures only one thread can access a critical section of code at a time. It acts like a lock that threads must acquire before entering protected code.

### Standard Properties

1. **Mutual Exclusion**: Only one thread can hold the lock
2. **Blocking**: Threads wait if lock is unavailable
3. **Ownership**: Lock is tied to the thread that acquired it
4. **Reentrancy**: Regular mutex is NOT reentrant (see `06_recursion_lock.cpp`)

## Understanding the Code

```cpp
#include <mutex>
#include <thread>

std::mutex m;
int myAmount = 0;

void addMoney() {
    m.lock();      // Acquire lock (blocks if already locked)
    myAmount++;    // Critical section - only one thread here
    m.unlock();    // Release lock
}

int main() {
    std::thread t1(addMoney);
    std::thread t2(addMoney);
    
    t1.join();
    t2.join();
    
    cout << myAmount << "\n";  // Always 2
}
```

### Execution Flow

```
Time    Thread 1              Thread 2
─────────────────────────────────────────
T1      lock() ✓
T2      [Critical Section]    (waiting...)
T3      myAmount++ (0→1)
T4      unlock()              lock() ✓
T5      (done)                [Critical Section]
T6                            myAmount++ (1→2)
T7                            unlock()
```

## Key Concepts

### Critical Section

The code between `lock()` and `unlock()` is called the **critical section**. This is the code that must be executed by only one thread at a time.

```cpp
m.lock();
// ← Critical Section starts
// Only one thread can execute this code
sharedData++;
// ← Critical Section ends
m.unlock();
```

### Mutex Operations

```cpp
std::mutex m;

m.lock();      // Acquire lock (blocks if unavailable)
m.unlock();    // Release lock
m.try_lock();  // Try to acquire without blocking (returns bool)
```

### Lock States

1. **Unlocked**: Available, any thread can acquire
2. **Locked**: Held by a thread, others must wait
3. **Blocked**: Threads waiting for the lock

## Complete Example with Multiple Threads

```cpp
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>

int sharedCounter = 0;
std::mutex mtx;

void incrementCounter(int threadID) {
    for (int i = 0; i < 5; i++) {
        mtx.lock();
        
        std::cout << "Thread " << threadID 
                  << " increments: " << sharedCounter 
                  << " → " << ++sharedCounter << "\n";
        
        mtx.unlock();
        
        // Simulate some work outside critical section
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::vector<std::thread> threads;
    
    // Create 3 threads
    for (int i = 0; i < 3; i++) {
        threads.push_back(std::thread(incrementCounter, i));
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final counter: " << sharedCounter << "\n";  // Always 15
    return 0;
}
```

## Performance Considerations

### Keep Critical Section Small

**Bad (holding lock too long):**
```cpp
m.lock();
int result = expensiveCalculation();  // Takes 10 seconds!
sharedData = result;
m.unlock();
```

**Good (lock only what's necessary):**
```cpp
int result = expensiveCalculation();  // Outside lock
m.lock();
sharedData = result;  // Quick operation
m.unlock();
```

### Lock Granularity

- **Fine-grained**: Many small locks (better parallelism, more complex)
- **Coarse-grained**: Few large locks (simpler, less parallelism)

## Common Mistakes ⚠️

### Mistake 1: Forgetting to Unlock

```cpp
// ❌ DEADLOCK!
m.lock();
std::cout << "Working...\n";
// Forgot unlock() - other threads wait forever!
```

**Solution**: Use RAII wrappers like `lock_guard` (see `07_lock_guard.cpp`)

### Mistake 2: Locking Wrong Resource

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

### Mistake 3: Double Locking

```cpp
// ❌ DEADLOCK with regular mutex!
m.lock();
m.lock();  // Same thread tries to lock again - deadlock!
```

**Solution**: Use `recursive_mutex` if needed (see `06_recursion_lock.cpp`)

## Thread-Safe Counter Class

```cpp
class ThreadSafeCounter {
private:
    int value = 0;
    mutable std::mutex mtx;  // mutable allows lock in const functions

public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        value++;
    }
    
    void decrement() {
        std::lock_guard<std::mutex> lock(mtx);
        value--;
    }
    
    int getValue() const {
        std::lock_guard<std::mutex> lock(mtx);
        return value;
    }
};

int main() {
    ThreadSafeCounter counter;
    
    std::thread t1([&counter]() {
        for (int i = 0; i < 1000; i++) counter.increment();
    });
    
    std::thread t2([&counter]() {
        for (int i = 0; i < 1000; i++) counter.increment();
    });
    
    t1.join();
    t2.join();
    
    std::cout << counter.getValue() << "\n";  // Always 2000
}
```

## Types of Mutexes in C++

| Type | Reentrant? | Timeout? | Use Case |
|------|-----------|----------|----------|
| `std::mutex` | No | No | General purpose |
| `std::recursive_mutex` | Yes | No | Recursive functions |
| `std::timed_mutex` | No | Yes | Need timeout |
| `std::recursive_timed_mutex` | Yes | Yes | Recursive + timeout |
| `std::shared_mutex` | Special | No | Reader-writer locks |

## Important Notes

⚠️ **Always unlock** - Forgetting causes deadlock  
⚠️ **Keep critical section small** - Don't hold lock unnecessarily  
⚠️ **One mutex per shared resource** - Clear ownership  
⚠️ **Exception safety** - Use RAII wrappers (see `07_lock_guard.cpp`)  
⚠️ **Lock ordering** - Always lock in same order to avoid deadlock

## Better Approach

Using `lock()` and `unlock()` manually is error-prone. Modern C++ uses RAII:

- **`std::lock_guard`**: Automatic unlock (see `07_lock_guard.cpp`)
- **`std::unique_lock`**: Manual control (see `08_unique_lock.cpp`)

## Next Steps

- Learn non-blocking locks: `04_try_lock.cpp`
- Learn automatic locking: `07_lock_guard.cpp`
- Learn recursive locking: `06_recursion_lock.cpp`
