# try_lock(): Non-Blocking Lock Attempts 🎯

## The Problem with Blocking Locks

When you call `mutex.lock()`, the thread **blocks** (waits) until the lock is available. Sometimes you don't want to wait:

```cpp
std::mutex mtx;

void blockingApproach() {
    mtx.lock();  // If busy, thread just sits and waits... doing nothing
    // Critical section
    mtx.unlock();
}
```

## The Solution: try_lock()

`try_lock()` attempts to acquire a lock **without waiting**. It returns immediately:

- `true` if the lock was successfully acquired
- `false` if the lock is already held by another thread

```cpp
#include <thread>
#include <mutex>
#include <iostream>

std::mutex mtx;
int counter = 0;

void tryLockExample() {
    for (int i = 0; i < 5; i++) {
        if (mtx.try_lock()) {  // Try to get the lock
            std::cout << "Got lock, incrementing counter\n";
            counter++;
            mtx.unlock();
        } else {
            std::cout << "Lock was busy, doing something else\n";
        }
    }
}

int main() {
    std::thread t1(tryLockExample);
    std::thread t2(tryLockExample);

    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << "\n";
    return 0;
}
```

## Comparison: lock() vs try_lock()

```cpp
// lock() - Blocking
mutex.lock();          // Wait here until available
criticalSection();     // Run this
mutex.unlock();

// try_lock() - Non-blocking
if (mutex.try_lock()) {
    criticalSection();  // Only if we got the lock
    mutex.unlock();
} else {
    doSomethingElse();  // Handle the case we didn't get it
}
```

## Use Cases

### 1. Avoid Deadlock Situations

```cpp
std::mutex m1, m2;

// ❌ RISKY - Could deadlock
void riskyFunction() {
    m1.lock();
    // ... do work ...
    m2.lock();  // What if another thread has m2 and waits for m1?
    // Deadlock!
}

// ✅ SAFER - Can avoid deadlock
void saferFunction() {
    if (m1.try_lock()) {
        if (m2.try_lock()) {
            // Both acquired, proceed
            m2.unlock();
        }
        m1.unlock();
    }
}
```

### 2. Responsive Programs (Don't Block User)

```cpp
void dataProcessor() {
    if (sharedDataMutex.try_lock()) {
        processData();  // Update the cache
        sharedDataMutex.unlock();
    }
    // If mutex busy, skip this iteration and keep going
    // UI stays responsive!
}
```

### 3. Exponential Backoff Retry

```cpp
bool acquireLockWithRetry(std::mutex& m, int maxAttempts = 10) {
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        if (m.try_lock()) {
            return true;  // Got the lock!
        }

        // Wait a bit longer each time before trying again
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1 << attempt)  // 1ms, 2ms, 4ms, 8ms...
        );
    }
    return false;  // Couldn't acquire after all attempts
}
```

## Real-World Example: Cache Update

```cpp
#include <thread>
#include <mutex>
#include <chrono>

struct DataCache {
    int value = 0;
    std::mutex mtx;

    void updateCache(int newValue) {
        // Try to update, but don't block if busy
        if (mtx.try_lock()) {
            std::cout << "Updating cache\n";
            value = newValue;
            mtx.unlock();
        } else {
            std::cout << "Cache locked, skipping update\n";
        }
    }

    int readCache() const {
        return value;  // Can read anytime
    }
};

int main() {
    DataCache cache;

    std::thread updater([]() {
        for (int i = 0; i < 3; i++) {
            cache.updateCache(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::thread reader([]() {
        for (int i = 0; i < 5; i++) {
            std::cout << "Cache value: " << cache.readCache() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    updater.join();
    reader.join();

    return 0;
}
```

## Common Patterns

### Pattern 1: Simple Try-Lock

```cpp
if (mtx.try_lock()) {
    // Do protected work
    mtx.unlock();
}
```

### Pattern 2: Try with Fallback

```cpp
if (mtx.try_lock()) {
    importantWork();
    mtx.unlock();
} else {
    lessImportantWork();  // Alternative when lock unavailable
}
```

### Pattern 3: Retry Loop

```cpp
while (!mtx.try_lock()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
// Got the lock!
mtx.unlock();
```

## Pitfalls ⚠️

### Mistake: Busy Waiting Without Sleep

```cpp
// ❌ BAD - Wastes CPU spinning in a loop
while (!mutex.try_lock()) {
    // Spinning: constantly retrying!
}

// ✅ GOOD - Add a sleep
while (!mutex.try_lock()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
```

### Mistake: Forgetting to Check Return Value

```cpp
// ❌ WRONG - Doesn't check if lock succeeded
mutex.try_lock();
sharedData++;  // Data might not be protected!
mutex.unlock();

// ✅ CORRECT
if (mutex.try_lock()) {
    sharedData++;
    mutex.unlock();
}
```

## try_lock() vs lock()

| Feature             | `lock()`        | `try_lock()`        |
| ------------------- | --------------- | ------------------- |
| **Blocking**        | Yes             | No                  |
| **Always acquires** | Yes (if called) | No                  |
| **Return value**    | Void            | bool                |
| **When to use**     | Must have lock  | Can proceed without |
| **Deadlock risk**   | Higher          | Lower               |

## Key Takeaways

✅ `try_lock()` returns immediately (true/false)  
✅ Prevents blocking if lock is unavailable  
✅ Useful for responsive programs  
✅ Can help avoid deadlocks  
✅ Always add `sleep` if retrying in a loop  
✅ Check the return value!

## Next Steps

- Learn about locking multiple mutexes safely (`05_multiple_try_lock.cpp`)
- Explore automatic lock guards (`07_lock_guard.cpp`)

---

**Difficulty Level**: ⭐⭐ Intermediate  
**Estimated Time**: 20-25 minutes  
**Prerequisites**: Mutex basics, deadlock concept
