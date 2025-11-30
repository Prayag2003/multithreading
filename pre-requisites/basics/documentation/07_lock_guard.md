# Lock Guard - Automatic Locking 🔐

## Overview

`std::lock_guard` is a **RAII** (Resource Acquisition Is Initialization) wrapper that automatically locks a mutex when created and unlocks it when destroyed. This ensures exception safety and prevents forgetting to unlock.

### Standard Definition

RAII is a C++ programming technique where resources (like locks) are tied to object lifetime. The resource is acquired in the constructor and released in the destructor, ensuring proper cleanup even if exceptions occur.

## The Problem: Manual Lock/Unlock

### Error-Prone Manual Locking

```cpp
std::mutex m;
int sharedData = 0;

void unsafeFunction() {
    m.lock();
    sharedData++;  // What if exception is thrown here?
    m.unlock();    // Never reached if exception occurs!
}
```

**Problems:**
- If exception is thrown, `unlock()` is never called
- Leads to **deadlock** - other threads wait forever
- Easy to forget `unlock()`
- Multiple return paths require multiple `unlock()` calls

### Example of Deadlock

```cpp
void problematic() {
    m.lock();
    if (someCondition) {
        return;  // ❌ Forgot unlock() - deadlock!
    }
    doSomething();
    m.unlock();
}
```

## The Solution: lock_guard

### How It Works

```cpp
std::mutex mu;
int buffer = 0;

void loop(const char *threadId, int loopFor) {
    std::lock_guard<std::mutex> lg(mu);  // Lock acquired here
    for (int i = 0; i < loopFor; i++) {
        buffer++;
        std::cout << threadId << " " << buffer << "\n";
    }
    // Lock automatically released when lg goes out of scope
}
```

### Execution Flow

1. **Construction**: `lock_guard` constructor calls `mutex.lock()`
2. **Usage**: Protected code executes
3. **Destruction**: `lock_guard` destructor calls `mutex.unlock()` automatically

### Exception Safety

```cpp
void safeFunction() {
    std::lock_guard<std::mutex> lg(m);
    sharedData++;  // Even if exception thrown here...
    throw std::runtime_error("Error");
    // ... unlock() is still called in destructor!
}
```

## Understanding the Code

```cpp
#include <mutex>
#include <thread>
#include <iostream>

std::mutex mu;
int buffer = 0;

void loop(const char *threadId, int loopFor) {
    // Lock guard is there for the respective scope
    std::lock_guard<std::mutex> lg(mu);
    
    for (int i = 0; i < loopFor; i++) {
        buffer++;
        std::cout << threadId << " " << buffer << "\n";
    }
    // lg destroyed here, mutex automatically unlocked
}

int main() {
    std::thread t1(loop, "Thread 1: ", 5);
    std::thread t2(loop, "Thread 2: ", 5);
    
    t1.join();
    t2.join();
    
    return 0;
}
```

### Output

```
Thread 1: 1
Thread 1: 2
Thread 1: 3
Thread 1: 4
Thread 1: 5
Thread 2: 6
Thread 2: 7
Thread 2: 8
Thread 2: 9
Thread 2: 10
```

Notice: Thread 1 completes entirely before Thread 2 starts (because of the lock).

## Key Benefits

### 1. Automatic Unlock

```cpp
// Manual (error-prone)
m.lock();
// ... code ...
m.unlock();  // Easy to forget!

// lock_guard (safe)
{
    std::lock_guard<std::mutex> lg(m);
    // ... code ...
}  // Automatically unlocks here
```

### 2. Exception Safety

```cpp
void function() {
    std::lock_guard<std::mutex> lg(m);
    riskyOperation();  // May throw
    anotherOperation(); // May throw
    // Unlock guaranteed even if exception thrown
}
```

### 3. Multiple Return Paths

```cpp
void function(int x) {
    std::lock_guard<std::mutex> lg(m);
    
    if (x < 0) return;      // ✅ Safe - unlocks automatically
    if (x > 100) return;    // ✅ Safe - unlocks automatically
    
    process(x);              // ✅ Safe - unlocks automatically
}
```

## Comparison

### Without lock_guard

```cpp
void unsafe() {
    m.lock();
    if (error) {
        return;  // ❌ Deadlock - forgot unlock()
    }
    doWork();
    m.unlock();
}
```

### With lock_guard

```cpp
void safe() {
    std::lock_guard<std::mutex> lg(m);
    if (error) {
        return;  // ✅ Safe - unlocks automatically
    }
    doWork();
    // ✅ Safe - unlocks automatically
}
```

## Scope-Based Locking

### Lock Duration

The lock is held for the **entire scope** where `lock_guard` exists:

```cpp
void example() {
    // No lock here
    
    {
        std::lock_guard<std::mutex> lg(m);
        // Lock held here
        sharedData++;
    }  // Lock released here
    
    // No lock here
    doOtherWork();
}
```

### Early Release (Not Possible)

```cpp
// ❌ Cannot manually unlock lock_guard
std::lock_guard<std::mutex> lg(m);
lg.unlock();  // Error: no unlock() method!
```

**Solution**: Use `unique_lock` if you need manual control (see `08_unique_lock.cpp`)

## Complete Examples

### Example 1: Thread-Safe Counter

```cpp
#include <mutex>
#include <thread>
#include <vector>

class SafeCounter {
private:
    int count = 0;
    mutable std::mutex mtx;  // mutable allows lock in const

public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        count++;
    }
    
    int getValue() const {
        std::lock_guard<std::mutex> lock(mtx);
        return count;
    }
};

SafeCounter counter;

void incrementMany(int times) {
    for (int i = 0; i < times; i++) {
        counter.increment();
    }
}

int main() {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; i++) {
        threads.push_back(std::thread(incrementMany, 1000));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final count: " << counter.getValue() << "\n";
    return 0;
}
```

### Example 2: Protected Data Structure

```cpp
#include <vector>
#include <mutex>

template<typename T>
class ThreadSafeVector {
private:
    std::vector<T> data;
    mutable std::mutex mtx;

public:
    void push_back(const T& item) {
        std::lock_guard<std::mutex> lock(mtx);
        data.push_back(item);
    }
    
    T pop_back() {
        std::lock_guard<std::mutex> lock(mtx);
        if (data.empty()) {
            throw std::runtime_error("Empty vector");
        }
        T item = data.back();
        data.pop_back();
        return item;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return data.size();
    }
};
```

### Example 3: Exception Safety

```cpp
void processData() {
    std::lock_guard<std::mutex> lg(mutex);
    
    try {
        riskyOperation1();  // May throw
        riskyOperation2();  // May throw
        riskyOperation3();  // May throw
    } catch (const std::exception& e) {
        // Lock still released even if exception caught
        std::cerr << "Error: " << e.what() << "\n";
    }
    // Lock released here (or in catch block)
}
```

## Important Notes

⚠️ **Cannot manually unlock** - Unlocks only when destroyed  
⚠️ **Cannot copy** - One lock_guard per mutex  
⚠️ **Scope-based** - Lock is held for the entire scope  
⚠️ **Exception safe** - Unlocks even if exception thrown  
⚠️ **Simple and fast** - Minimal overhead

## When to Use

### ✅ Use lock_guard when:

- Simple automatic locking is enough
- Lock scope matches function/block scope
- You want exception safety
- You don't need manual control
- Performance is important (slightly faster than unique_lock)

### ❌ Don't use lock_guard when:

- You need condition variables (use `unique_lock`)
- You need manual unlock/lock
- You need deferred locking
- You need to transfer lock ownership

## Comparison with Other Mechanisms

| Feature | Manual lock/unlock | lock_guard | unique_lock |
|---------|-------------------|-----------|-------------|
| Automatic unlock | ❌ | ✅ | ✅ |
| Exception safe | ❌ | ✅ | ✅ |
| Manual control | ✅ | ❌ | ✅ |
| Condition variables | ❌ | ❌ | ✅ |
| Performance | - | Fastest | Slightly slower |

## Best Practices

✅ **Always use lock_guard** for simple cases  
✅ **Prefer over manual lock/unlock** - Safer and cleaner  
✅ **Use appropriate scope** - Keep lock scope minimal  
✅ **One lock_guard per mutex** - Don't try to copy  
✅ **Combine with other RAII** - Consistent resource management

## Common Mistakes

### Mistake 1: Trying to Unlock Manually

```cpp
// ❌ Wrong
std::lock_guard<std::mutex> lg(m);
lg.unlock();  // Error: no unlock() method
```

### Mistake 2: Copying lock_guard

```cpp
// ❌ Wrong
std::lock_guard<std::mutex> lg1(m);
auto lg2 = lg1;  // Error: not copyable
```

### Mistake 3: Holding Lock Too Long

```cpp
// ⚠️ Not ideal - lock held during expensive operation
void notIdeal() {
    std::lock_guard<std::mutex> lg(m);
    expensiveComputation();  // Lock held unnecessarily
    sharedData = result;
}

// ✅ Better - minimize lock scope
void better() {
    int result = expensiveComputation();  // Outside lock
    {
        std::lock_guard<std::mutex> lg(m);
        sharedData = result;  // Quick operation
    }
}
```

## Advanced: Adopt Lock

```cpp
// If you already have the lock
m.lock();
std::lock_guard<std::mutex> lg(m, std::adopt_lock);
// Takes ownership of already-locked mutex
```

## Next Steps

- Learn manual control: `08_unique_lock.cpp`
- Learn about mutex basics: `03_mutex.cpp`
