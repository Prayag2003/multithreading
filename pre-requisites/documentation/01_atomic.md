# Atomics: Lock-Free Synchronization ⚛️

## Problem: Shared Primitives

Regular integers aren't thread-safe:

```cpp
int counter = 0;

// ❌ NOT atomic
counter++;  // Multiple threads corrupt data
```

## Solution: std::atomic

Atomic operations are thread-safe without locks:

```cpp
#include <atomic>
#include <thread>

std::atomic<int> counter(0);

void increment() {
    for (int i = 0; i < 1000; i++) {
        counter++;  // ✅ Thread-safe, no mutex!
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << "\n";  // Always 2000
    return 0;
}
```

## Common Operations

```cpp
std::atomic<int> x(0);

x++;              // Increment (atomic)
x--;              // Decrement (atomic)
x += 5;           // Add (atomic)
x.store(10);      // Set value
int val = x.load(); // Get value
int old = x.exchange(20);  // Set and return old
```

## Compare and Swap

```cpp
std::atomic<int> x(10);

// If x == 10, set to 20, return true
bool success = x.compare_exchange_strong(10, 20);
```

## Benefits Over Mutex

```cpp
// With mutex
std::mutex mtx;
{
    std::lock_guard<std::mutex> lock(mtx);
    counter++;
}

// With atomic (simpler, faster)
std::atomic<int> counter;
counter++;
```

| Feature          | Mutex        | Atomic       |
| ---------------- | ------------ | ------------ |
| **Speed**        | Slower       | Faster       |
| **Code**         | More verbose | Simpler      |
| **Memory order** | Implicit     | Customizable |
| **Complex ops**  | Easy         | Limited      |

## When to Use

✅ **Atomic**: Simple shared variables (counters, flags)  
✅ **Mutex**: Complex operations, multiple variables

## Example: Flag

```cpp
std::atomic<bool> ready(false);

void worker() {
    ready = true;  // ✅ Thread-safe
}

int main() {
    std::thread t(worker);

    while (!ready) {  // ✅ Thread-safe read
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    t.join();
    std::cout << "Ready!\n";
    return 0;
}
```

## Key Takeaways

✅ Atomics provide lock-free synchronization  
✅ Faster than mutex for simple operations  
✅ Work with primitive types  
✅ Easy to use: just use like normal variables  
✅ Perfect for counters and flags
