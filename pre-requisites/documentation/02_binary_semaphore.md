# Binary Semaphore 🚦

## What is a Semaphore?

A counter that controls access to a resource.

- **Binary Semaphore**: Counter is 0 or 1 (like a mutex)
- **Semaphore**: Counter can be any value

## Simple Example

```cpp
#include <semaphore>
#include <thread>
#include <iostream>

std::binary_semaphore sem(1);  // Start with 1

void worker(int id) {
    std::cout << "Worker " << id << " waiting\n";
    sem.acquire();  // Decrement, wait if 0

    std::cout << "Worker " << id << " working\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Worker " << id << " done\n";
    sem.release();  // Increment
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    t1.join();
    t2.join();

    return 0;
}
```

## How It Works

```
Semaphore Counter
Initial: 1

Thread 1: acquire() → Counter: 0 (enters)
Thread 2: acquire() → Wait (counter is 0)

Thread 1: release() → Counter: 1
Thread 2: acquire() → Counter: 0 (enters now)
```

## Comparison with Mutex

```cpp
// Mutex-like (binary semaphore)
std::binary_semaphore sem(1);
sem.acquire();
// Critical section
sem.release();

// Mutex equivalent
std::mutex mtx;
mtx.lock();
// Critical section
mtx.unlock();
```

| Feature         | Mutex           | Binary Semaphore  |
| --------------- | --------------- | ----------------- |
| **Lock/Unlock** | Yes             | acquire/release   |
| **Ownership**   | Thread-specific | No ownership      |
| **Use**         | General sync    | Resource counting |

## Non-Blocking Try

```cpp
std::binary_semaphore sem(1);

if (sem.try_acquire()) {
    // Got access
    sem.release();
} else {
    // Not available
}
```

## Timeout

```cpp
if (sem.try_acquire_for(std::chrono::milliseconds(100))) {
    // Got it
    sem.release();
} else {
    // Timeout
}
```

## Key Takeaways

✅ Binary semaphore is counter-based lock  
✅ `acquire()` decrements (waits if 0)  
✅ `release()` increments  
✅ Lighter than mutex in some cases  
✅ No thread ownership (any thread can release)

## Note

Binary semaphores are similar to mutexes but:

- Don't require owner to unlock
- Can be released by different thread
- Use with careful design
