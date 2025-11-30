# Unique Lock - Manual Control 🔓

## Overview

`std::unique_lock` is like `lock_guard` but with more flexibility. It allows manual locking/unlocking and is required for condition variables.

## Understanding the Code

The example shows `unique_lock` being used with condition variables. Here's the basic usage:

```cpp
std::mutex mut;
std::unique_lock<std::mutex> ul(mut);  // Automatically locks

// Can manually unlock if needed
ul.unlock();

// Can manually lock again
ul.lock();
```

## Key Features

1. **Automatic locking** (like lock_guard)
2. **Manual unlock/lock** (unlike lock_guard)
3. **Deferred locking** - Don't lock immediately
4. **Required for condition variables**

## Comparison with lock_guard

| Feature | lock_guard | unique_lock |
|---------|-----------|-------------|
| Automatic unlock | ✅ | ✅ |
| Manual unlock | ❌ | ✅ |
| Deferred locking | ❌ | ✅ |
| Performance | Faster | Slightly slower |
| Flexibility | Less | More |

## Deferred Locking

```cpp
std::unique_lock<std::mutex> ul(mut, std::defer_lock);
// Mutex NOT locked yet!

ul.lock();  // Lock manually when ready
// ... do work ...
ul.unlock();  // Unlock manually
```

## With Condition Variables

```cpp
std::condition_variable cv;
std::unique_lock<std::mutex> ul(mut);

cv.wait(ul, []() { return condition; });
// wait() automatically unlocks and relocks
```

**Why unique_lock?** Condition variables need to unlock the mutex while waiting, which `lock_guard` cannot do.

## When to Use

**Use unique_lock when:**
- You need condition variables
- You need manual control over locking
- You need deferred locking
- You need to transfer lock ownership

**Use lock_guard when:**
- Simple automatic locking is enough
- You want better performance
- Lock scope matches block scope

## Next Steps

- Learn condition variables: `09_conditional_variable.cpp`
