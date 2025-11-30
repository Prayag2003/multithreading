# Binary Semaphore - Simple Signaling 🚦

## Overview

A binary semaphore is like a traffic light with two states: available (1) or unavailable (0). It's used for signaling between threads.

## Understanding the Code

```cpp
std::binary_semaphore smpSignalMainToThread{0};  // Start at 0 (unavailable)
std::binary_semaphore smpSignalThreadToMain{0};   // Start at 0 (unavailable)

void ThreadFunc() {
    smpSignalMainToThread.acquire();  // Wait for signal from main
    cout << "[Thread]: Got the signal\n";
    this_thread::sleep_for(3s);
    cout << "[Thread]: Send the signal\n";
    smpSignalThreadToMain.release();  // Signal main thread
}

int main() {
    thread worker(ThreadFunc);
    cout << "[Main]: Send the signal\n";
    smpSignalMainToThread.release();  // Signal worker to start
    
    smpSignalThreadToMain.acquire();  // Wait for worker to finish
    worker.join();
    cout << "[Main] Got the signal";
}
```

## How It Works

### Semaphore Operations

- **acquire()**: Decrements count (waits if count is 0)
- **release()**: Increments count (signals waiting threads)

### The Flow

1. Main creates thread (worker waits)
2. Main calls `release()` → signals worker
3. Worker calls `acquire()` → gets signal, proceeds
4. Worker does work, then calls `release()` → signals main
5. Main calls `acquire()` → gets signal, continues

## Key Concepts

**Binary semaphore:**
- Count can only be 0 or 1
- Used for signaling (not resource counting)
- Similar to condition variable but simpler

**Initial value:**
- `{0}`: Unavailable, must be released first
- `{1}`: Available, can be acquired immediately

## When to Use

**Use binary semaphore when:**
- Simple thread signaling
- One thread needs to wait for another
- Simple handshake pattern

**Use condition variable when:**
- Need to wait for complex conditions
- Need predicate checking
- More control needed

## Comparison

| Feature | Binary Semaphore | Condition Variable |
|---------|------------------|-------------------|
| Simplicity | ✅ Simpler | More complex |
| Predicate | ❌ No | ✅ Yes |
| Mutex needed | ❌ No | ✅ Yes |
| Use case | Simple signaling | Complex conditions |

## Important Notes

⚠️ **C++20 feature** - Requires C++20 compiler  
⚠️ **Initial value matters** - Choose based on your needs  
⚠️ **acquire/release pairs** - Must match

## Next Steps

- See producer-consumer with semaphores: `03_producer_consumer_semaphore.cpp`
