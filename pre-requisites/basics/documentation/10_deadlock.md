# Deadlock - What It Is and How to Avoid It ⚠️

## Overview

A deadlock occurs when two or more threads are waiting for each other to release locks, causing all threads to be stuck forever.

## Understanding the Code

The example demonstrates a deadlock scenario:

```cpp
std::mutex m1, m2;

void thread1() {
    m1.lock();
    m2.lock();  // Waits if thread2 has m2
    // ... work ...
    m1.unlock();
    m2.unlock();
}

void thread2() {
    m2.lock();
    m1.lock();  // Waits if thread1 has m1
    // ... work ...
    m2.unlock();
    m1.unlock();
}
```

**What happens:**
- Thread 1 locks m1, then tries to lock m2
- Thread 2 locks m2, then tries to lock m1
- Both threads wait forever → **DEADLOCK!**

## How Deadlocks Happen

1. **Circular wait**: Thread 1 waits for Thread 2, Thread 2 waits for Thread 1
2. **Different lock order**: Threads acquire locks in different order
3. **Multiple locks**: Need more than one mutex

## Solutions

### Solution 1: Consistent Lock Order

Always acquire locks in the same order:

```cpp
// Both threads do this:
m1.lock();  // Always lock m1 first
m2.lock();  // Then lock m2
```

### Solution 2: Use std::lock()

```cpp
std::lock(m1, m2);  // Locks both atomically, avoids deadlock
std::lock_guard<std::mutex> lg1(m1, std::adopt_lock);
std::lock_guard<std::mutex> lg2(m2, std::adopt_lock);
```

### Solution 3: Use std::try_lock()

```cpp
int result = std::try_lock(m1, m2);
if (result == -1) {
    // Successfully locked both
    // ... work ...
    m1.unlock();
    m2.unlock();
}
```

## Prevention Tips

✅ **Always lock in same order**  
✅ **Use std::lock() for multiple locks**  
✅ **Use lock_guard/unique_lock** (automatic unlock)  
✅ **Avoid nested locks when possible**  
✅ **Use timeout locks** (try_lock_for)

## Detection

If your program hangs and threads are waiting, you might have a deadlock. Use debugging tools to identify it.

## Next Steps

- Learn about futures and promises: `11_future_and_promise.cpp`
