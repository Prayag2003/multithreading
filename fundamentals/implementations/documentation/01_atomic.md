# Atomics - Lock-Free Synchronization ⚛️

## Overview

`std::atomic` provides thread-safe operations on primitive types without using mutexes. Atomic operations are guaranteed to be indivisible and are typically faster than mutex-based synchronization for simple operations.

### Standard Definition

An atomic operation is one that is guaranteed to be executed as a single, indivisible unit. No other thread can observe a partially completed atomic operation. Atomic operations are the foundation of lock-free programming.

## The Problem: Non-Atomic Operations

Regular operations on primitive types are not atomic:

```cpp
int counter = 0;

// This is NOT atomic:
counter++;  // Actually: load → increment → store (3 steps)
```

**What can go wrong:**
```
Thread 1: Load(0) → Increment → Store(1)
Thread 2: Load(0) → Increment → Store(1)  // Lost update!
Result: 1 (should be 2)
```

## The Solution: std::atomic

Atomic operations are guaranteed to be:
- **Indivisible**: Cannot be interrupted
- **Thread-safe**: Multiple threads can safely modify
- **Lock-free**: No mutex overhead
- **Visible**: Changes are immediately visible to other threads

## Understanding the Code

```cpp
#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> counter;  // Atomic counter
int times = 100000;

void run() {
    for (int i = 0; i < times; i++) {
        counter++;  // Thread-safe, no mutex needed!
    }
}

int main() {
    std::thread t1(run);
    std::thread t2(run);
    
    t1.join();
    t2.join();
    
    std::cout << counter << "\n";  // Always 200000
    return 0;
}
```

### Why It Works

The `++` operation on `std::atomic<int>` is guaranteed to be atomic. The CPU ensures that:
1. The read-modify-write happens as one operation
2. No other thread can interfere
3. The result is always correct

## Key Operations

### Basic Operations

```cpp
std::atomic<int> x(0);

x++;              // Atomic increment
x--;              // Atomic decrement
x += 5;           // Atomic add
x -= 3;           // Atomic subtract
x = 10;           // Atomic assignment (via operator=)
```

### Explicit Load/Store

```cpp
std::atomic<int> x(0);

x.store(10);           // Atomic store
int val = x.load();    // Atomic load

// Equivalent to:
x = 10;
int val = x;
```

### Exchange

```cpp
std::atomic<int> x(10);

int old = x.exchange(20);  // Set to 20, return old value
// x is now 20, old is 10
```

### Compare and Swap (CAS)

```cpp
std::atomic<int> x(10);

// Compare-and-swap: if x == expected, set to desired
int expected = 10;
bool success = x.compare_exchange_strong(expected, 20);
// If x was 10, it's now 20 and success is true
// If x wasn't 10, expected is updated to actual value
```

## Comparison with Mutex

### With Mutex (More Overhead)

```cpp
#include <mutex>

std::mutex mtx;
int counter = 0;

void increment() {
    std::lock_guard<std::mutex> lock(mtx);
    counter++;  // Protected but slower
}
```

**Overhead:**
- Lock acquisition (may block)
- Cache line contention
- Context switching if blocked

### With Atomic (Faster)

```cpp
#include <atomic>

std::atomic<int> counter(0);

void increment() {
    counter++;  // Thread-safe and fast
}
```

**Benefits:**
- No blocking (usually)
- Hardware-level atomicity
- Better cache performance
- Lower latency

### Performance Comparison

| Operation | Mutex | Atomic | Speedup |
|-----------|-------|--------|---------|
| Simple increment | ~100ns | ~10ns | ~10x |
| Complex operation | Mutex required | Not applicable | - |

## Memory Ordering

### Default: Sequentially Consistent

```cpp
std::atomic<int> x(0);
std::atomic<int> y(0);

// Thread 1
x.store(1);  // Sequentially consistent (default)
y.store(1);

// Thread 2
if (y.load() == 1) {
    assert(x.load() == 1);  // Always true with seq_cst
}
```

### Other Ordering Options

```cpp
// Relaxed ordering (fastest, least guarantees)
x.store(1, std::memory_order_relaxed);

// Acquire-release (common for locks)
x.store(1, std::memory_order_release);
int val = y.load(std::memory_order_acquire);
```

**For beginners**: Use default (sequentially consistent) unless you need performance optimization.

## When to Use Atomic

### ✅ Use Atomic When:

- **Simple operations**: Increment, decrement, load, store
- **Single variable**: One variable needs protection
- **Performance critical**: Need maximum speed
- **Counters/flags**: Simple shared state
- **Lock-free data structures**: Building lock-free algorithms

### ❌ Don't Use Atomic When:

- **Complex operations**: Multiple statements need to be atomic together
- **Multiple variables**: Need to protect multiple variables atomically
- **Condition variables**: Need to wait for conditions
- **Complex critical sections**: Use mutex instead

## Complete Examples

### Example 1: Thread-Safe Counter

```cpp
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

std::atomic<int> counter(0);

void increment(int times) {
    for (int i = 0; i < times; i++) {
        counter++;
    }
}

int main() {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; i++) {
        threads.push_back(std::thread(increment, 10000));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Counter: " << counter << "\n";  // Always 40000
    return 0;
}
```

### Example 2: Flag Synchronization

```cpp
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> ready(false);

void worker() {
    // Do some work
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    ready = true;  // Signal completion
}

int main() {
    std::thread t(worker);
    
    // Wait for flag
    while (!ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "Worker is ready!\n";
    t.join();
    return 0;
}
```

### Example 3: Atomic Pointer

```cpp
#include <atomic>

struct Data {
    int value;
    // ... other fields
};

std::atomic<Data*> shared_ptr(nullptr);

void writer() {
    Data* new_data = new Data{42};
    shared_ptr.store(new_data);  // Atomic pointer assignment
}

void reader() {
    Data* ptr = shared_ptr.load();  // Atomic pointer read
    if (ptr != nullptr) {
        std::cout << "Value: " << ptr->value << "\n";
    }
}
```

## Supported Types

### Built-in Types

```cpp
std::atomic<int>          // Integer
std::atomic<bool>         // Boolean
std::atomic<char>         // Character
std::atomic<long>         // Long integer
std::atomic<void*>        // Pointer
// ... and other integral types
```

### Custom Types

```cpp
struct Point {
    int x, y;
};

// Must be trivially copyable
std::atomic<Point> point;  // If Point is trivially copyable
```

## Important Notes

⚠️ **Only for simple types** - Works with integers, pointers, booleans  
⚠️ **Not for complex operations** - Use mutex for multi-step operations  
⚠️ **Memory ordering** - Default is sequentially consistent (safe but may be slower)  
⚠️ **Cache coherency** - Atomic operations ensure cache coherency  
⚠️ **Lock-free guarantee** - Atomic operations are typically lock-free (but not guaranteed)

## Common Patterns

### Pattern 1: Counter

```cpp
std::atomic<int> count(0);
count++;  // Thread-safe increment
```

### Pattern 2: Flag

```cpp
std::atomic<bool> flag(false);
flag = true;  // Thread-safe assignment
if (flag) { /* ... */ }
```

### Pattern 3: Producer-Consumer Index

```cpp
std::atomic<int> producer_index(0);
std::atomic<int> consumer_index(0);

// Producer
int idx = producer_index.fetch_add(1);  // Get and increment

// Consumer
int idx = consumer_index.fetch_add(1);
```

## Benefits Over Mutex

| Feature | Mutex | Atomic |
|---------|-------|--------|
| **Speed** | Slower | Faster |
| **Blocking** | May block | Usually non-blocking |
| **Code** | More verbose | Simpler |
| **Complexity** | Easy to use | Need to understand memory ordering |
| **Use case** | Complex operations | Simple operations |

## Limitations

1. **Only simple operations**: Can't protect complex multi-step operations
2. **Single variable**: Each atomic protects one variable
3. **No waiting**: Can't wait for conditions (use condition variables)
4. **Memory ordering complexity**: Advanced usage requires understanding memory models

## Best Practices

✅ **Use for counters and flags**  
✅ **Use default memory ordering** (unless optimizing)  
✅ **Prefer over mutex for simple operations**  
✅ **Understand the limitations**  
✅ **Test thoroughly** - Lock-free code can have subtle bugs

## Next Steps

- Learn about mutexes: `pre-requisites/basics/03_mutex.cpp`
- Learn about semaphores: `02_binary_semaphore.cpp`
