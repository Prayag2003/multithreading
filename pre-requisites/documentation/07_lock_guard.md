# Lock Guard: RAII Mutex Protection 🛡️

## The Problem: Manual Lock/Unlock

Manual lock management is error-prone:

```cpp
std::mutex mtx;

void riskyFunction() {
    mtx.lock();

    // What if an exception happens here?
    riskyOperation();  // throws!

    mtx.unlock();  // Never reached! Deadlock!
}
```

## The Solution: std::lock_guard

`std::lock_guard` uses RAII (Resource Acquisition Is Initialization) to automatically unlock:

```cpp
#include <thread>
#include <mutex>

std::mutex mtx;

void safeFunction() {
    std::lock_guard<std::mutex> guard(mtx);  // Lock acquired

    // Exception happens here
    riskyOperation();  // throws!

    // guard destroyed here, lock AUTOMATICALLY released
    // Exception-safe!
}
```

## How lock_guard Works

```
Constructor: Calls lock()
   ↓
[Critical Section]  ← Exclusive access
   ↓
Destructor: Calls unlock()  ← ALWAYS happens!
```

**Key Feature**: Destructor is called even if an exception occurs!

## Basic Usage

```cpp
#include <thread>
#include <mutex>
#include <iostream>

std::mutex mtx;
int counter = 0;

void incrementCounter() {
    std::lock_guard<std::mutex> guard(mtx);  // Auto lock

    counter++;
    std::cout << "Counter: " << counter << "\n";

}  // Auto unlock here

int main() {
    std::thread t1(incrementCounter);
    std::thread t2(incrementCounter);

    t1.join();
    t2.join();

    return 0;
}
```

**Output:**

```
Counter: 1
Counter: 2
```

Always predictable and thread-safe!

## Scope-Based Protection

The lock is held for the scope duration:

```cpp
std::mutex mtx;
int sharedData = 0;

void scopedLocking() {
    std::cout << "Before lock\n";

    {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Inside lock\n";
        sharedData++;
    }  // Lock released here

    std::cout << "After lock\n";
}
```

**Output:**

```
Before lock
Inside lock
After lock
```

## Exception Safety Guarantee

```cpp
#include <thread>
#include <mutex>
#include <stdexcept>

std::mutex mtx;
int resource = 0;

void exceptionSafeExample() {
    std::lock_guard<std::mutex> guard(mtx);

    resource = 1;

    throw std::runtime_error("Oops!");  // Exception occurs

    resource = 2;  // Never reached

}  // Lock automatically released before unwinding!

int main() {
    try {
        exceptionSafeExample();
    } catch (const std::runtime_error& e) {
        std::cout << "Caught: " << e.what() << "\n";
        std::cout << "But lock was released safely!\n";
    }
    return 0;
}
```

## Practical Example: Thread-Safe Vector

```cpp
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>

class ThreadSafeVector {
    std::vector<int> data;
    mutable std::mutex mtx;

public:
    void push(int value) {
        std::lock_guard<std::mutex> guard(mtx);  // Lock
        data.push_back(value);
    }  // Unlock

    int pop() {
        std::lock_guard<std::mutex> guard(mtx);  // Lock
        if (data.empty()) {
            throw std::runtime_error("Empty!");
        }
        int value = data.back();
        data.pop_back();
        return value;
    }  // Unlock

    size_t size() const {
        std::lock_guard<std::mutex> guard(mtx);  // Lock for read too!
        return data.size();
    }  // Unlock
};

int main() {
    ThreadSafeVector vec;

    std::thread t1([&]() {
        for (int i = 0; i < 5; i++) {
            vec.push(i);
        }
    });

    std::thread t2([&]() {
        for (int i = 5; i < 10; i++) {
            vec.push(i);
        }
    });

    t1.join();
    t2.join();

    std::cout << "Vector size: " << vec.size() << "\n";

    return 0;
}
```

## Comparison: Manual vs lock_guard

```cpp
// ❌ Manual Lock Management
void manualLocking() {
    mtx.lock();
    try {
        doWork();
    } catch (...) {
        mtx.unlock();  // Must remember
        throw;
    }
    mtx.unlock();  // Must remember
}

// ✅ lock_guard
void guardedLocking() {
    std::lock_guard<std::mutex> guard(mtx);
    doWork();  // Exception-safe!
}
```

## Key Differences from Manual Locks

| Feature               | Manual | lock_guard |
| --------------------- | ------ | ---------- |
| **Lock on creation**  | No     | Yes        |
| **Unlock on destroy** | Manual | Automatic  |
| **Exception safe**    | ❌ No  | ✅ Yes     |
| **Scope based**       | ❌ No  | ✅ Yes     |
| **RAII pattern**      | ❌ No  | ✅ Yes     |

## Nested Locks

```cpp
std::mutex mtx1, mtx2;

void nestedLocks() {
    std::lock_guard<std::mutex> guard1(mtx1);

    {
        std::lock_guard<std::mutex> guard2(mtx2);
        // Both locked here
    }  // mtx2 released

    // mtx1 still locked

}  // mtx1 released
```

## Common Patterns

### Pattern 1: Simple Protection

```cpp
void updateSharedData(int value) {
    std::lock_guard<std::mutex> guard(mtx);
    sharedData = value;
}
```

### Pattern 2: Scoped Protection

```cpp
void partiallySharedFunction() {
    // Unprotected section
    int local = expensiveComputation();

    {
        std::lock_guard<std::mutex> guard(mtx);
        sharedData = local;  // Only critical part locked
    }

    // Unprotected section
    std::cout << "Done\n";
}
```

### Pattern 3: Class Member Locking

```cpp
class ThreadSafeClass {
    int value = 0;
    mutable std::mutex mtx;

public:
    void setValue(int v) {
        std::lock_guard<std::mutex> guard(mtx);
        value = v;
    }

    int getValue() const {
        std::lock_guard<std::mutex> guard(mtx);
        return value;
    }
};
```

## Performance Note

lock_guard has **minimal overhead** compared to manual locking:

- Constructor: `lock()`
- Destructor: `unlock()`

No busy-waiting or extra operations. It's **zero-cost abstraction**.

## When NOT to Use lock_guard

❌ When you need temporary unlock and relock (→ use `unique_lock`)  
❌ When you need timed locking (→ use `unique_lock`)  
❌ When using condition variables (→ use `unique_lock`)

## Best Practices

✅ Always use `lock_guard` instead of manual lock/unlock  
✅ Keep critical sections small  
✅ Use `mutable` for locking in const methods  
✅ Lock latest, unlock earliest  
✅ Nest guards properly with scopes  
✅ Prefer this RAII pattern for all mutex usage

## Key Takeaways

✅ `lock_guard` automatically locks in constructor, unlocks in destructor  
✅ Exception-safe by design (RAII pattern)  
✅ Scope-based lifetime  
✅ Zero-cost abstraction  
✅ Simplifies code and prevents deadlocks  
✅ For most situations, prefer `lock_guard` over manual locking

## Next Steps

- Learn about `unique_lock` (`08_unique_lock.cpp`) for advanced scenarios
- Explore condition variables (`09_conditional_variable.cpp`)

---

**Difficulty Level**: ⭐⭐ Intermediate  
**Estimated Time**: 20-25 minutes  
**Prerequisites**: Mutex basics, RAII pattern, exceptions
