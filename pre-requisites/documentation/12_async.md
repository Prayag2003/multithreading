# Async: Simplest Concurrency 🚀

## The Easy Way: std::async

Instead of creating threads manually, let `std::async` handle it:

```cpp
#include <future>
#include <iostream>

int compute(int a, int b) {
    return a + b;
}

int main() {
    // Create task automatically
    std::future<int> result = std::async(compute, 5, 3);

    // Get result (waits if not ready)
    std::cout << "Result: " << result.get() << "\n";

    return 0;
}
```

**That's it!** No manual threads or promises.

## Comparison: Manual vs Async

```cpp
// ❌ Manual (tedious)
std::promise<int> p;
auto f = p.get_future();
std::thread t([pr = std::move(p)]() mutable {
    pr.set_value(compute(5, 3));
});
int result = f.get();
t.join();

// ✅ Async (clean)
auto result = std::async(compute, 5, 3);
int value = result.get();
```

## Launch Policies

### Eager (Default)

```cpp
std::async(std::launch::async, compute, 5, 3);
// Starts immediately in new thread
```

### Lazy

```cpp
std::async(std::launch::deferred, compute, 5, 3);
// Runs when you call .get()
```

### Either (Policy Choice)

```cpp
std::async(std::launch::async | std::launch::deferred, compute, 5, 3);
// System decides
```

## Practical Example: Parallel Computation

```cpp
#include <future>
#include <vector>
#include <numeric>

int computeSum(const std::vector<int>& data) {
    return std::accumulate(data.begin(), data.end(), 0);
}

int main() {
    std::vector<int> data1 = {1, 2, 3, 4, 5};
    std::vector<int> data2 = {6, 7, 8, 9, 10};

    // Run both simultaneously
    auto f1 = std::async(computeSum, data1);
    auto f2 = std::async(computeSum, data2);

    int total = f1.get() + f2.get();  // Wait for both
    std::cout << "Total: " << total << "\n";

    return 0;
}
```

## With Lambdas

```cpp
auto result = std::async([]() {
    std::cout << "Running async\n";
    return 42;
});

std::cout << "Value: " << result.get() << "\n";
```

## Exception Handling

Exceptions propagate through future:

```cpp
auto f = std::async([]() {
    throw std::runtime_error("Error!");
    return 0;
});

try {
    int value = f.get();  // Rethrows!
} catch (const std::runtime_error& e) {
    std::cout << "Caught: " << e.what() << "\n";
}
```

## Multiple Tasks

```cpp
std::vector<std::future<int>> tasks;

for (int i = 0; i < 5; i++) {
    tasks.push_back(std::async([i]() {
        return i * 10;
    }));
}

int total = 0;
for (auto& task : tasks) {
    total += task.get();
}
```

## Key Advantages

✅ **Simple**: No manual thread management  
✅ **Clean**: Less boilerplate code  
✅ **Safe**: Exception-safe by design  
✅ **Flexible**: Works with functions, lambdas, methods  
✅ **Returns values**: Get results directly

## When to Use

| Choose             | When                             |
| ------------------ | -------------------------------- |
| **std::async**     | Simple tasks, need return values |
| **std::thread**    | Full control, long-lived threads |
| **promise/future** | Complex synchronization          |

## Quick Reference

```cpp
// Function
auto f = std::async(myFunction, arg1, arg2);

// Lambda
auto f = std::async([]() { return 42; });

// Get result (blocks)
int result = f.get();

// Check status
if (f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
    int value = f.get();
}
```

## Key Takeaways

✅ `std::async` simplest way to run tasks  
✅ Returns `std::future` with result  
✅ `.get()` waits and returns value  
✅ Launch policies control execution  
✅ Exception-safe  
✅ Prefer over manual threads for simple cases

## Next Steps

- Explore producer-consumer implementations
- Study thread pool design
- Learn about atomics and lock-free programming

---

**Difficulty Level**: ⭐ Beginner-Intermediate  
**Estimated Time**: 15-20 minutes  
**Prerequisites**: Futures, return values, lambdas
