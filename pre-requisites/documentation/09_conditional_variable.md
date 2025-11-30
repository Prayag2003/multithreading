# Condition Variables: Thread Synchronization 🔔

## The Producer-Consumer Problem

How do threads communicate and coordinate?

```cpp
// Without condition variables - INEFFICIENT!
int data = 0;
bool ready = false;

void producer() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    data = 42;
    ready = true;
}

void consumer() {
    // Busy-wait - wastes CPU cycles!
    while (!ready) {
        // Spin, checking constantly
    }
    std::cout << "Received: " << data << "\n";
}
```

**Problem**: Consumer wastes CPU spinning until data arrives.

## The Solution: std::condition_variable

A condition variable allows threads to **signal** when something happens:

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

std::mutex mtx;
std::condition_variable cv;
int data = 0;
bool ready = false;

void producer() {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        std::unique_lock<std::mutex> lock(mtx);
        data = 42;
        ready = true;
    }
    cv.notify_one();  // Signal waiting threads
}

void consumer() {
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, []() { return ready; });  // Sleep until ready!

    std::cout << "Received: " << data << "\n";
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}
```

**Benefits**: Consumer sleeps instead of spinning, saving CPU!

## How Condition Variables Work

```
Consumer                          Producer
   │                                 │
wait() - Enter sleep
   │                                 │
   │ (CPU released!)                 │
   │                                 ├─ Do work
   │                                 │
   │                             notify_one()
   │ (Woken up!)                     │
   │                                 │
Resume ─ Check predicate             │
   │                                 │
Process data ──────────────────────►
```

## Key Operations

### wait() - Sleep Until Notified

```cpp
std::unique_lock<std::mutex> lock(mtx);

// Sleep until predicate is true
cv.wait(lock, []() { return ready; });

// Execution resumes here when notified AND ready == true
```

### notify_one() - Wake One Thread

```cpp
{
    std::unique_lock<std::mutex> lock(mtx);
    data = 42;
    ready = true;
}
cv.notify_one();  // Wake one waiting thread
```

### notify_all() - Wake All Threads

```cpp
{
    std::unique_lock<std::mutex> lock(mtx);
    data = 42;
    ready = true;
}
cv.notify_all();  // Wake all waiting threads
```

## Important: Always Use a Predicate

❌ **WRONG** - Spurious wakeup!

```cpp
cv.wait(lock);  // Wakes up even without notification!
```

✅ **CORRECT** - Check condition

```cpp
cv.wait(lock, []() { return ready; });  // Only proceed if ready
```

**Why?** Operating systems may wake threads spuriously. The predicate protects against this.

## Practical Example: Bounded Buffer

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>

const int BUFFER_SIZE = 3;
std::queue<int> buffer;
std::mutex mtx;
std::condition_variable notFull, notEmpty;

void producer() {
    for (int item = 1; item <= 10; item++) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait if buffer is full
        notFull.wait(lock, []() { return buffer.size() < BUFFER_SIZE; });

        buffer.push(item);
        std::cout << "Produced: " << item << " (size: " << buffer.size() << ")\n";

        notEmpty.notify_one();  // Wake consumer
    }
}

void consumer() {
    int item;
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait if buffer is empty
        notEmpty.wait(lock, []() { return !buffer.empty(); });

        item = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << item << " (size: " << buffer.size() << ")\n";

        notFull.notify_one();  // Wake producer
    }
}

int main() {
    std::thread prod(producer);
    // Consumer runs separately

    prod.join();
    return 0;
}
```

## Pattern: Producer-Consumer

```
Data Flow:
Producer → [Buffer] → Consumer

Synchronization:
- Producer notifies when data available
- Consumer notifies when space available
- Both check predicates
```

## Common Patterns

### Pattern 1: Simple Signal

```cpp
std::condition_variable cv;
std::mutex mtx;
bool flag = false;

// Thread 1
cv.wait(lock, []() { return flag; });

// Thread 2
{
    std::unique_lock<std::mutex> lock(mtx);
    flag = true;
}
cv.notify_all();
```

### Pattern 2: Resource Ready

```cpp
std::condition_variable cv;
std::mutex mtx;
std::optional<Data> result;

void worker() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []() { return result.has_value(); });
    process(*result);
}

void producer() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        result = computeData();
    }
    cv.notify_one();
}
```

### Pattern 3: Multiple Waiters

```cpp
// Multiple consumers waiting
std::thread c1(consumer);
std::thread c2(consumer);
std::thread c3(consumer);

// Producer wakes all
cv.notify_all();
```

## Visual Timeline

```
Consumer 1       Consumer 2       Producer
   │                 │               │
wait() ─────────► sleep             │
   │                 │               │
   │              wait() ─────────► sleep
   │                 │               │
   │                 │ (1 second)    │
   │                 │               │
   │                 │            do work
   │                 │            flag = true
   │                 │        notify_all()
   │                 │            │
wake! ◄─────────────────────────────┘
   │                 │
check predicate:     │
   yes (flag=true)   │
   │                 │
proceed              │
                   wake!
                     │
                  check predicate:
                     yes (flag=true)
                     │
                  proceed
```

## notify_one() vs notify_all()

```cpp
// Use notify_one() when:
// - Only one thread needs to wake
// - Multiple consumers working independently

// Use notify_all() when:
// - Multiple threads need to act
// - State changed that affects many threads
```

## Example: Barrier Pattern

All threads wait for signal:

```cpp
std::condition_variable cv;
std::mutex mtx;
bool start = false;

void worker(int id) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []() { return start; });  // All wait here
    std::cout << "Worker " << id << " starting\n";
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.push_back(std::thread(worker, i));
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    {
        std::unique_lock<std::mutex> lock(mtx);
        start = true;
    }
    cv.notify_all();  // All start simultaneously

    for (auto& t : threads) t.join();
    return 0;
}
```

## Key Takeaways

✅ Condition variables allow thread signaling  
✅ `wait()` sleeps until notified  
✅ **Always use a predicate** with wait()  
✅ `notify_one()` wakes one thread  
✅ `notify_all()` wakes all threads  
✅ Requires `std::unique_lock` (not lock_guard!)  
✅ Efficient - threads don't busy-wait  
✅ Essential for producer-consumer patterns

## Common Mistakes ⚠️

### Mistake 1: No Predicate

```cpp
// ❌ WRONG
cv.wait(lock);

// ✅ CORRECT
cv.wait(lock, []() { return condition; });
```

### Mistake 2: Not Holding Lock During Notify

```cpp
// ❌ RISKY
ready = true;
cv.notify_one();  // Not protected!

// ✅ CORRECT
{
    std::unique_lock<std::mutex> lock(mtx);
    ready = true;
}
cv.notify_one();
```

### Mistake 3: Using lock_guard

```cpp
// ❌ WRONG - lock_guard doesn't support wait()
std::lock_guard<std::mutex> lock(mtx);
cv.wait(lock);  // Compile error!

// ✅ CORRECT
std::unique_lock<std::mutex> lock(mtx);
cv.wait(lock);
```

## Next Steps

- Learn about deadlocks (`10_deadlock.cpp`)
- Explore futures and promises (`11_future_and_promise.cpp`)
- Study async patterns (`12_async.cpp`)

---

**Difficulty Level**: ⭐⭐⭐ Intermediate-Advanced  
**Estimated Time**: 30-40 minutes  
**Prerequisites**: Mutex, unique_lock, producer-consumer concept
