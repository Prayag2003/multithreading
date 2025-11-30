# Unique Lock: Flexible Locking 🔓

## When lock_guard Isn't Enough

`std::lock_guard` is great but has limitations:

```cpp
std::mutex mtx;
std::lock_guard<std::mutex> guard(mtx);

// Can't unlock and relock
guard.unlock();  // ❌ Not available!

// Can't defer locking
std::mutex mtx2;
// ❌ Can't create guard without immediately locking
```

## The Solution: std::unique_lock

`std::unique_lock` provides **flexibility** while maintaining RAII safety:

```cpp
#include <thread>
#include <mutex>

std::mutex mtx;

void flexibleLocking() {
    std::unique_lock<std::mutex> lock(mtx);  // Lock acquired

    // Do work
    std::cout << "Working...\n";

    lock.unlock();  // Manual unlock available!

    // Unprotected section
    std::cout << "Outside critical section\n";

    lock.lock();  // Re-lock available!

    // Protected again
    std::cout << "Back in critical section\n";

}  // Automatic unlock in destructor
```

## Comparison: lock_guard vs unique_lock

| Feature                | `lock_guard` | `unique_lock`   |
| ---------------------- | ------------ | --------------- |
| **Auto lock/unlock**   | ✅ Yes       | ✅ Yes          |
| **Manual unlock**      | ❌ No        | ✅ Yes          |
| **Manual lock**        | ❌ No        | ✅ Yes          |
| **Deferred locking**   | ❌ No        | ✅ Yes          |
| **Timed locking**      | ❌ No        | ✅ Yes          |
| **Ownership transfer** | ❌ No        | ✅ Yes          |
| **Performance**        | Faster       | Slightly slower |

## Key Features

### 1. Deferred Locking

Create lock without acquiring immediately:

```cpp
std::mutex mtx;

void deferredLocking() {
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
    // mtx NOT locked yet

    std::cout << "Preparing...\n";

    lock.lock();  // Now lock
    // Critical section

}  // Auto unlock
```

### 2. Manual Control

```cpp
std::unique_lock<std::mutex> lock(mtx);

if (someCondition) {
    lock.unlock();  // Explicit unlock
}

// Later...
if (needsLock) {
    lock.lock();  // Re-lock
}
```

### 3. Timed Locking

Try to acquire lock with timeout:

```cpp
std::unique_lock<std::mutex> lock(mtx, std::chrono::milliseconds(100));

if (lock.owns_lock()) {
    std::cout << "Got lock within timeout\n";
} else {
    std::cout << "Timeout - couldn't acquire lock\n";
}
```

### 4. Ownership Transfer (Move Semantics)

```cpp
std::unique_lock<std::mutex> lock1(mtx);
// lock1 owns the lock

std::unique_lock<std::mutex> lock2 = std::move(lock1);
// lock2 now owns it, lock1 doesn't

// lock2 will unlock in its destructor
```

## Locking Strategies

```cpp
std::unique_lock<std::mutex> lock(mtx, strategy);
```

| Strategy           | Behavior                  |
| ------------------ | ------------------------- |
| `std::defer_lock`  | Don't lock yet            |
| `std::try_to_lock` | Try locking (don't wait)  |
| `std::adopt_lock`  | Already locked externally |

## Practical Examples

### Example 1: Try-Lock Pattern

```cpp
#include <thread>
#include <mutex>
#include <iostream>

std::mutex mtx;

void tryLockExample() {
    std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);

    if (lock) {  // Check if locked
        std::cout << "Got lock!\n";
        // Do work
    } else {
        std::cout << "Couldn't get lock, doing alternative\n";
    }
}  // Unlocks if owns lock

int main() {
    std::thread t1(tryLockExample);
    std::thread t2(tryLockExample);

    t1.join();
    t2.join();

    return 0;
}
```

### Example 2: Conditional Unlock

```cpp
std::mutex mtx;
int value = 0;

void conditionalWork() {
    std::unique_lock<std::mutex> lock(mtx);

    if (value > 10) {
        lock.unlock();  // Release early
        expensiveCalculation();  // Do outside lock
    } else {
        // Keep lock
        quickUpdate();
    }
}
```

### Example 3: Temporary Release

```cpp
std::unique_lock<std::mutex> lock(mtx);

std::cout << "Protected: " << value << "\n";

lock.unlock();
std::cout << "Unprotected work\n";
lock.lock();

std::cout << "Protected again: " << value << "\n";
```

## With Condition Variables (Very Common Use!)

This is where `unique_lock` is **essential**:

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void waiter() {
    std::unique_lock<std::mutex> lock(mtx);

    // Wait releases lock while waiting!
    cv.wait(lock, []() { return ready; });

    std::cout << "Ready!\n";
    // Lock re-acquired here
}

void notifier() {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        std::unique_lock<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_all();
}

int main() {
    std::thread t1(waiter);
    std::thread t2(notifier);

    t1.join();
    t2.join();

    return 0;
}
```

**Note**: `condition_variable::wait()` only works with `unique_lock`!

## Ownership Methods

```cpp
std::unique_lock<std::mutex> lock(mtx);

if (lock.owns_lock()) {
    std::cout << "Lock is held\n";
}

if (lock) {  // Implicit conversion
    std::cout << "Lock is held\n";
}

lock.release();  // Give up ownership (doesn't unlock!)
```

## State Checking

```cpp
std::unique_lock<std::mutex> lock(mtx, std::defer_lock);

bool owns = lock.owns_lock();  // false
lock.lock();
owns = lock.owns_lock();       // true

// Check mutex
std::mutex* m = lock.mutex();  // Get pointer to mutex
```

## Moving Locks

```cpp
std::unique_lock<std::mutex> lock1(mtx);

// Move to another lock
std::unique_lock<std::mutex> lock2 = std::move(lock1);

// lock1 no longer owns the lock
// lock2 will unlock in its destructor
```

## Common Patterns

### Pattern 1: RAII with Manual Unlock

```cpp
std::unique_lock<std::mutex> lock(mtx);
doWork();
lock.unlock();
unexpectedFunction();  // Won't crash program
lock.lock();
```

### Pattern 2: Condition Variable Wait

```cpp
std::unique_lock<std::mutex> lock(mtx);
cv.wait(lock, predicate);  // Releases and reacquires lock
```

### Pattern 3: Try-Lock

```cpp
std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
if (lock) {
    // Have lock
}
```

## When to Use

✅ **Use `unique_lock` when:**

- Working with condition variables
- Need manual unlock/relock
- Need try-lock semantics
- Need timed locking

✅ **Use `lock_guard` when:**

- Simple protection needed
- Don't need manual control
- Want lighter weight

## Performance

- `lock_guard`: Fast, minimal overhead
- `unique_lock`: Slightly slower (state tracking)

For most code, the difference is negligible.

## Key Takeaways

✅ `unique_lock` provides manual lock control  
✅ Deferred locking with `std::defer_lock`  
✅ Try-locking with `std::try_to_lock`  
✅ Essential for condition variables  
✅ Manual unlock/relock available  
✅ Move semantics for ownership transfer  
✅ Still RAII safe (auto unlock)

## Next Steps

- Learn about condition variables (`09_conditional_variable.cpp`)
- Explore deadlock prevention (`10_deadlock.cpp`)

---

**Difficulty Level**: ⭐⭐ Intermediate  
**Estimated Time**: 25-35 minutes  
**Prerequisites**: lock_guard, condition variables (for full understanding)
