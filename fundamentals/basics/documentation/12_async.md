# std::async - Simplest Concurrency 🚀

## Overview

`std::async` is the simplest way to run a function in the background and get its result. It's a high-level wrapper over promise/future that's much easier to use.

## Understanding the Code

The example shows two launch policies:

### 1. Deferred (Lazy) - Runs in Same Thread

```cpp
std::future<ull> oddSum = std::async(std::launch::deferred, findOddSum, start, end);
cout << "OddSum: " << oddSum.get() << "\n";  // Runs NOW in main thread
```

**Behavior:**
- Function doesn't run until you call `get()`
- Runs in the calling thread (main thread)
- **No parallelism** - just deferred execution

### 2. Async (Eager) - Runs in New Thread

```cpp
std::future<ull> oddSumAsync = std::async(std::launch::async, findOddSum, start, end);
cout << "OddSum: " << oddSumAsync.get() << "\n";  // Waits for result
```

**Behavior:**
- Function runs immediately in a new thread
- True parallelism
- `get()` blocks until result is ready

## Launch Policies

| Policy | When It Runs | Where It Runs |
|--------|-------------|---------------|
| `std::launch::deferred` | When you call `get()` | Same thread |
| `std::launch::async` | Immediately | New thread |
| Default (both) | Implementation decides | May or may not be parallel |

## Comparison with Promise/Future

**With promise/future (more code):**
```cpp
std::promise<ull> p;
std::future<ull> f = p.get_future();
std::thread t(worker, std::move(p), args);
ull result = f.get();
```

**With std::async (simpler):**
```cpp
std::future<ull> f = std::async(std::launch::async, worker, args);
ull result = f.get();
```

## Key Benefits

✅ **Simple syntax** - One line to run async  
✅ **Automatic thread management** - No need to create/join threads  
✅ **Exception safe** - Exceptions propagate automatically  
✅ **Returns future** - Easy to get results

## Important Notes

⚠️ **Default policy is implementation-defined** - May or may not create thread  
⚠️ **Always specify policy** - Use `std::launch::async` for guaranteed parallelism  
⚠️ **get() blocks** - Waits for result  
⚠️ **Destructor waits** - Future destructor waits if async policy used

## When to Use

**Use std::async when:**
- You want simple parallel execution
- You need return values from threads
- You want automatic thread management

**Use std::thread when:**
- You need more control
- You don't need return values
- You want to manage thread lifecycle manually

## Next Steps

- Explore implementations: `pre-requisites/implementations/`
