# Condition Variables - Thread Signaling 📡

## Overview

**Condition variables** are synchronization primitives that allow threads to wait for a specific condition to become true and notify other threads when the condition changes. They provide an efficient way to coordinate threads without busy-waiting.

### Standard Definition

A condition variable is a synchronization object that enables threads to wait until a particular condition is met. It must be used together with a mutex to avoid race conditions.

## The Problem: Busy-Waiting

Without condition variables, threads often use busy-waiting (polling):

```cpp
// ❌ Inefficient busy-waiting
while (buffer.empty()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // Wastes CPU checking repeatedly!
}
```

**Problems:**

- Wastes CPU cycles
- Adds unnecessary delay
- Doesn't respond immediately when condition changes

## The Solution: Condition Variables

Condition variables allow threads to:

1. **Wait efficiently**: Sleep until condition is met
2. **Notify others**: Wake up waiting threads when condition changes
3. **Avoid polling**: No need to repeatedly check conditions

## Understanding the Code

The example demonstrates a bank account scenario:

```cpp
#include <condition_variable>
#include <mutex>
#include <thread>

int balance = 0;
std::condition_variable cv;
std::mutex mut;

void withDrawMoney(int amount) {
    std::unique_lock<std::mutex> ul(mut);

    // Wait for balance to be non-zero
    cv.wait(ul, []() {
        return (balance != 0) ? true : false;
    });

    // Balance is now non-zero, proceed with withdrawal
    if (balance >= amount) {
        balance -= amount;
        cout << "Amount deducted: " << amount << "\n";
    } else {
        cout << "Insufficient balance\n";
    }
    cout << "Balance: " << balance << "\n";
}

void addMoney(int amount) {
    std::lock_guard<std::mutex> lg(mut);
    balance += amount;
    cout << "Amount added: " << amount << "\n";
    cout << "Balance = " << balance << "\n";

    cv.notify_one();  // Notify waiting thread
}

int main() {
    std::thread withdrawThread(withDrawMoney, 500);
    std::thread addThread(addMoney, 500);

    withdrawThread.join();
    addThread.join();
}
```

### Execution Flow

```
Time    Withdraw Thread          Add Thread
─────────────────────────────────────────────
T1      lock() ✓
T2      wait() → unlock()        (waiting...)
T3      (sleeping...)            lock() ✓
T4      (sleeping...)            balance += 500
T5      (sleeping...)            notify_one()
T6      (wakes up)               unlock()
T7      lock() ✓
T8      Check condition ✓
T9      Withdraw 500
T10     unlock()
```

## How Condition Variables Work

### wait() Operation

```cpp
cv.wait(ul, predicate);
```

**Steps:**

1. Unlocks the mutex (allows other threads to proceed)
2. Waits for notification (thread sleeps, no CPU waste)
3. When notified, relocks the mutex
4. Checks the predicate
5. If predicate is false, goes back to step 2
6. If predicate is true, continues execution

### notify_one() vs notify_all()

```cpp
cv.notify_one();   // Wakes up ONE waiting thread
cv.notify_all();   // Wakes up ALL waiting threads
```

**When to use:**

- `notify_one()`: Only one thread can proceed (e.g., single consumer)
- `notify_all()`: Multiple threads can proceed (e.g., multiple consumers)

## Key Concepts

### The Predicate (Lambda Function)

```cpp
cv.wait(ul, []() { return balance != 0; });
```

The predicate is a function that returns `true` when the condition is met.

**Why it's needed:**

1. **Spurious wakeups**: Threads may wake up without notification
2. **Multiple conditions**: Check if YOUR condition is met
3. **State verification**: Ensure data is in correct state

### Spurious Wakeups

**Definition**: A thread may wake up from `wait()` even if no thread called `notify()`. This is allowed by the standard for performance reasons.

**Solution**: Always use a predicate to verify the condition:

```cpp
// ❌ Wrong - may proceed even if condition not met
cv.wait(ul);

// ✅ Correct - verifies condition
cv.wait(ul, []() { return balance != 0; });
```

### Why unique_lock?

Condition variables require `unique_lock`, not `lock_guard`, because:

- `wait()` needs to unlock the mutex
- `lock_guard` cannot be unlocked manually
- `unique_lock` allows manual unlock/lock

## Complete Example: Producer-Consumer

```cpp
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv;
const int MAX_SIZE = 10;

void producer() {
    for (int i = 0; i < 20; i++) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until buffer has space
        cv.wait(lock, []() { return buffer.size() < MAX_SIZE; });

        buffer.push(i);
        std::cout << "Produced: " << i << "\n";

        lock.unlock();
        cv.notify_one();  // Notify consumer
    }
}

void consumer() {
    for (int i = 0; i < 20; i++) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until buffer has items
        cv.wait(lock, []() { return !buffer.empty(); });

        int value = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << value << "\n";

        lock.unlock();
        cv.notify_one();  // Notify producer
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();
}
```

## Common Patterns

### Pattern 1: Wait for Condition

```cpp
// Consumer pattern
{
    std::unique_lock<std::mutex> ul(mutex);
    cv.wait(ul, []() { return dataReady; });
    // Use data
}
```

### Pattern 2: Notify After Change

```cpp
// Producer pattern
{
    std::lock_guard<std::mutex> lg(mutex);
    // Modify shared data
    data = newValue;
    dataReady = true;
}
cv.notify_one();  // Notify after unlocking (better performance)
```

### Pattern 3: Multiple Conditions

```cpp
std::condition_variable cv1, cv2;

// Thread 1
cv1.wait(ul, []() { return condition1; });

// Thread 2
cv2.wait(ul, []() { return condition2; });
```

## Important Notes

⚠️ **Always use unique_lock** - `lock_guard` cannot unlock  
⚠️ **Always use predicate** - Prevents spurious wakeups  
⚠️ **Lock before modifying** - Protect shared data with mutex  
⚠️ **Notify after unlocking** - Better performance (optional but recommended)  
⚠️ **One condition variable per condition** - Clear semantics

## Performance Tips

1. **Notify after unlock**: Slightly better performance

      ```cpp
      {
          lock_guard<mutex> lg(mtx);
          data = value;
      }
      cv.notify_one();  // After unlock
      ```

2. **Use notify_all() sparingly**: Only when multiple threads can proceed

3. **Keep predicates simple**: Complex predicates reduce performance

## Comparison with Other Mechanisms

| Mechanism              | Use Case           | Efficiency            |
| ---------------------- | ------------------ | --------------------- |
| **Condition Variable** | Wait for condition | ✅ Efficient (sleeps) |
| **Busy-waiting**       | Simple polling     | ❌ Wastes CPU         |
| **Semaphore**          | Resource counting  | ✅ Efficient          |
| **Mutex only**         | Simple protection  | ❌ No signaling       |

## Common Mistakes

### Mistake 1: No Predicate

```cpp
// ❌ Wrong - spurious wakeups possible
cv.wait(ul);
```

### Mistake 2: Wrong Lock Type

```cpp
// ❌ Wrong - lock_guard cannot unlock
std::lock_guard<std::mutex> lg(mtx);
cv.wait(lg, []() { return ready; });  // Compile error!
```

### Mistake 3: Notifying Without Lock

```cpp
// ⚠️ Works but not ideal
cv.notify_one();  // Without holding lock
// Better to notify after unlock for performance
```

## Next Steps

- Learn about deadlocks: `10_deadlock.cpp`
- See producer-consumer implementation: `fundamentals/implementations/00_producer_consumer_mutex.cpp`
