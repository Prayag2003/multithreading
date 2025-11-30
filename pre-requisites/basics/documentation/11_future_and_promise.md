# Future and Promise - Getting Results from Threads 📬

## Overview

`std::promise` and `std::future` allow a thread to return a value (or exception) to another thread. This is how you get results from background threads.

## Understanding the Code

The example shows:
- A worker thread calculating the sum of odd numbers
- Using promise to send the result back
- Using future to receive the result in main thread

```cpp
void findOddSum(std::promise<ull> &&OddSumPromise, ull start, ull end) {
    ull OddSum = 0;
    for (ull i = start; i <= end; i++) {
        if (i & 1) OddSum += i;
    }
    OddSumPromise.set_value(OddSum);  // Send result back
}

int main() {
    std::promise<ull> OddSumPromise;
    std::future<ull> OddSumFuture = OddSumPromise.get_future();
    
    std::thread t1(findOddSum, std::move(OddSumPromise), start, end);
    
    ull result = OddSumFuture.get();  // Wait for and get result
}
```

## How It Works

1. **Create promise**: `std::promise<T> promise`
2. **Get future**: `std::future<T> future = promise.get_future()`
3. **Move promise to thread**: Thread will set the value
4. **Call get()**: Blocks until value is available, then returns it

## Key Concepts

### Promise
- **Producer side**: Thread that computes the value
- **set_value()**: Sets the result
- **set_exception()**: Sets an exception

### Future
- **Consumer side**: Thread that needs the result
- **get()**: Waits for and retrieves the value
- **wait()**: Just waits, doesn't retrieve

## Important Notes

⚠️ **get() can only be called once** - After get(), future is invalid  
⚠️ **Promise must be moved** - Promises are not copyable  
⚠️ **get() blocks** - Waits until value is set  
⚠️ **Exception handling** - Exceptions set via promise are rethrown in get()

## Exception Handling

```cpp
try {
    ull result = OddSumFuture.get();
} catch (const std::exception& ex) {
    // Handle exception from worker thread
}
```

## When to Use

**Use promise/future when:**
- You need a return value from a thread
- You need exception propagation
- You want explicit control

**Use std::async when:**
- You want simpler syntax (see `12_async.cpp`)

## Next Steps

- Learn simpler alternative: `12_async.cpp`
