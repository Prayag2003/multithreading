# Futures and Promises: Return Values from Threads 🎁

## The Problem

How do you get a result back from a thread?

```cpp
// ❌ This doesn't work - threads don't return values!
std::thread t([]() {
    return 42;  // Can't capture return value!
});
```

## The Solution: std::promise and std::future

**Promise**: Set a value  
**Future**: Get the value (waits if not ready)

```cpp
#include <thread>
#include <future>
#include <iostream>

int main() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    std::thread t([p = std::move(promise)]() mutable {
        std::cout << "Working...\n";
        p.set_value(42);  // Send result
    });

    int result = future.get();  // Wait and receive
    std::cout << "Result: " << result << "\n";

    t.join();
    return 0;
}
```

## How It Works

```
Thread 1              Thread 2
promise ────────────► Set value
   ↑                      │
   │                      │
future.get() ◄───────── Value
(Wait)
   │
Resume with value
```

## Simple Example: Calculate in Thread

```cpp
#include <thread>
#include <future>

int computeSum(int a, int b) {
    return a + b;
}

int main() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    std::thread t([p = std::move(promise)]() mutable {
        int result = computeSum(10, 20);
        p.set_value(result);
    });

    std::cout << "Sum: " << future.get() << "\n";  // Blocks until ready

    t.join();
    return 0;
}
```

## Exception Handling

Pass exceptions through future:

```cpp
std::promise<int> promise;
std::future<int> future = promise.get_future();

std::thread t([p = std::move(promise)]() mutable {
    try {
        // Some work
        throw std::runtime_error("Error!");
    } catch (...) {
        p.set_exception(std::current_exception());
    }
});

try {
    int result = future.get();  // Rethrows exception!
} catch (const std::runtime_error& e) {
    std::cout << "Caught: " << e.what() << "\n";
}

t.join();
```

## Checking Status

```cpp
std::future<int> future = ...;

if (future.valid()) {
    std::cout << "Future is valid\n";
}

std::future_status status = future.wait_for(std::chrono::milliseconds(100));
if (status == std::future_status::ready) {
    int value = future.get();
} else if (status == std::future_status::timeout) {
    std::cout << "Not ready yet\n";
}
```

## Multiple Values

Use `std::packaged_task` for cleaner code:

```cpp
#include <thread>
#include <future>

int add(int a, int b) {
    return a + b;
}

int main() {
    std::packaged_task<int(int, int)> task(add);
    std::future<int> future = task.get_future();

    std::thread t(std::move(task), 5, 3);

    std::cout << "Result: " << future.get() << "\n";

    t.join();
    return 0;
}
```

## Key Differences

| Method             | Use When                              |
| ------------------ | ------------------------------------- |
| **promise/future** | Custom control over when value is set |
| **packaged_task**  | Wrapping existing functions           |
| **async**          | Simplest approach (next topic)        |

## Common Patterns

### Pattern 1: Single Result

```cpp
std::promise<int> p;
auto f = p.get_future();
std::thread t([pr = std::move(p)]() mutable { pr.set_value(42); });
int result = f.get();
```

### Pattern 2: Exception Passing

```cpp
try {
    // Risky operation
} catch (...) {
    promise.set_exception(std::current_exception());
}
```

### Pattern 3: Multiple Threads

```cpp
std::vector<std::future<int>> futures;
for (int i = 0; i < 5; i++) {
    std::promise<int> p;
    futures.push_back(p.get_future());
    std::thread([pr = std::move(p)]() mutable {
        pr.set_value(i * 10);
    }).detach();
}

for (auto& f : futures) {
    std::cout << f.get() << "\n";  // Blocks until ready
}
```

## Key Takeaways

✅ Promise: Set value from thread  
✅ Future: Get value (waits if not ready)  
✅ Exception-safe: Can pass errors through  
✅ Check status before getting  
✅ Move-only (can't copy promises)

## Next Steps

- Learn about `std::async` (`12_async.cpp`) - simpler approach
- Explore more complex patterns

---

**Difficulty Level**: ⭐⭐⭐ Intermediate-Advanced  
**Estimated Time**: 25-35 minutes  
**Prerequisites**: Threads, exception handling, move semantics
