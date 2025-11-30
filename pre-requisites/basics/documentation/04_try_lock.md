# try_lock() - Non-Blocking Lock 🔓

## Overview

`try_lock()` attempts to acquire a lock without blocking. If the lock is available, it acquires it. If not, it returns immediately without waiting.

## Understanding the Code

```cpp
std::mutex mu;
int counter = 0;

void increaseCounterTo10000() {
    for (int i = 0; i < 10000; i++) {
        if (mu.try_lock()) {  // Try to acquire lock
            counter++;         // Only if lock acquired
            mu.unlock();
        }
        // If lock not acquired, skip this iteration
    }
}
```

## How try_lock() Works

- **Returns `true`**: Lock was acquired successfully
- **Returns `false`**: Lock was not available (another thread has it)

## Key Differences

| Method | Behavior |
|--------|----------|
| `lock()` | Blocks until lock is available |
| `try_lock()` | Returns immediately (non-blocking) |

## When to Use try_lock()

**Use when:**
- You don't want to wait for a lock
- You have alternative work to do if lock is busy
- You want to avoid potential deadlocks

**In the example:**
- Each thread tries to increment the counter 10,000 times
- If the lock is busy, it skips that attempt
- Final counter value may be less than 20,000 (because some attempts fail)

## Important Notes

⚠️ **try_lock() may fail** - Always check the return value  
⚠️ **Not suitable for critical operations** - Use regular `lock()` when you must succeed  
⚠️ **Can lead to starvation** - Some threads might never get the lock

## Next Steps

- Learn to lock multiple mutexes: `05_multiple_try_lock.cpp`
