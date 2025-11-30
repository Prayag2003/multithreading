# Multiple Locks - std::try_lock() 🔐

## Overview

When you need to lock multiple mutexes, you must be careful to avoid deadlocks. `std::try_lock()` helps by trying to lock multiple mutexes at once.

## Understanding the Code

The example demonstrates:
- Two threads incrementing X and Y separately
- A consumer thread that needs both X and Y to be non-zero
- Using `std::try_lock()` to safely acquire both locks

## std::try_lock() Function

```cpp
int result = std::try_lock(mu1, mu2);
```

**Returns:**
- `-1`: Successfully locked all mutexes
- `0` or higher: Index of the first mutex that couldn't be locked

## How It Works

```cpp
void consumeXorY() {
    int lockResult = std::try_lock(mu1, mu2);
    if (lockResult == -1) {
        // Successfully locked both mutexes
        // Do work...
        mu1.unlock();
        mu2.unlock();
    }
    // If failed, try again later
}
```

## Key Points

1. **Atomic operation**: Either all mutexes are locked, or none
2. **No deadlock risk**: The function handles lock ordering internally
3. **Non-blocking**: Returns immediately if it can't acquire all locks

## Why This Matters

**Problem without std::try_lock():**
```cpp
// Thread 1
mu1.lock();
mu2.lock();  // Deadlock if Thread 2 has mu2!

// Thread 2  
mu2.lock();
mu1.lock();  // Deadlock if Thread 1 has mu1!
```

**Solution with std::try_lock():**
- Safely attempts to lock both at once
- Avoids deadlock by using internal ordering

## Important Notes

⚠️ **Always unlock in reverse order** (or use RAII like lock_guard)  
⚠️ **Check return value** - Only proceed if all locks acquired  
⚠️ **May need retry logic** - If try_lock fails, try again later

## Next Steps

- Learn about deadlocks: `10_deadlock.cpp`
- Learn automatic locking: `07_lock_guard.cpp`
