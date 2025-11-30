# Recursive Mutex 🔄

## Overview

A recursive mutex allows the same thread to lock the mutex multiple times. This is useful in recursive functions or when a function calls another function that needs the same lock.

## Understanding the Code

```cpp
std::recursive_mutex recursive_mut;
int criticalVariable = 0;

void recursion(const char *desc, int loop) {
    if (loop < 0) return;
    
    recursive_mut.lock();  // Lock acquired
    cout << desc << " | Value: " << criticalVariable++ << "\n";
    recursion(desc, loop - 1);  // Recursive call - needs same lock!
    recursive_mut.unlock();  // Must unlock as many times as locked
}
```

## Key Difference

| Type | Can Same Thread Lock Again? |
|------|----------------------------|
| `std::mutex` | ❌ No - causes deadlock |
| `std::recursive_mutex` | ✅ Yes - allows multiple locks |

## How It Works

1. Thread locks the recursive mutex (lock count = 1)
2. Same thread locks again (lock count = 2)
3. Thread unlocks once (lock count = 1)
4. Thread unlocks again (lock count = 0) - now other threads can acquire

## Important Rule

⚠️ **Unlock as many times as you lock!**

If you lock 3 times, you must unlock 3 times. Otherwise, other threads will never be able to acquire the lock.

## When to Use

**Use recursive_mutex when:**
- You have recursive functions that need the same lock
- A function calls another function that needs the same lock
- You're not sure about the call chain

**Otherwise, use regular mutex** - it's faster and simpler.

## Example Use Case

```cpp
void functionA() {
    recursive_mut.lock();
    functionB();  // Also needs the lock
    recursive_mut.unlock();
}

void functionB() {
    recursive_mut.lock();  // OK with recursive_mutex!
    // Do work
    recursive_mut.unlock();
}
```

## Next Steps

- Learn automatic locking: `07_lock_guard.cpp`
