# Deadlock: Detecting and Preventing 🚫

## What is Deadlock?

Deadlock happens when threads wait for each other in a circular pattern, causing the program to **freeze forever**.

```
Thread A: Waiting for resource B (held by Thread B)
Thread B: Waiting for resource A (held by Thread A)
Result: Both frozen forever!
```

## Classic Example

```cpp
std::mutex m1, m2;

void threadA() {
    m1.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m2.lock();  // Wait forever for m2 (B has it!)
}

void threadB() {
    m2.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m1.lock();  // Wait forever for m1 (A has it!)
}

int main() {
    std::thread t1(threadA);
    std::thread t2(threadB);

    t1.join();  // Program hangs here forever!
    t2.join();
}
```

## Prevention Strategies

### 1. Lock Ordering

Always acquire locks in the **same order**:

```cpp
// ✅ CORRECT
void threadA() {
    m1.lock();
    m2.lock();
    // Work
    m2.unlock();
    m1.unlock();
}

void threadB() {
    m1.lock();  // Always first, just like A
    m2.lock();
    // Work
    m2.unlock();
    m1.unlock();
}
```

### 2. Use std::lock()

Acquires multiple locks safely:

```cpp
std::lock(m1, m2);  // Deadlock-proof
std::lock_guard<std::mutex> g1(m1, std::adopt_lock);
std::lock_guard<std::mutex> g2(m2, std::adopt_lock);
```

### 3. Scoped Lock (C++17)

Simplest modern approach:

````cpp
std::scoped_lock lock(m1, m2);  // Safe, automatic
// Work
```  // Auto unlock

### 4. Keep Critical Sections Short
Less time holding locks = less deadlock chance:

```cpp
// ❌ Bad
{
    std::lock_guard<std::mutex> lock(mtx);
    expensiveOperation();  // Takes 10 seconds!
}

// ✅ Good
expensiveOperation();  // Do outside lock
{
    std::lock_guard<std::mutex> lock(mtx);
    updateResult();  // Quick operation
}
````

### 5. Avoid Nested Locks When Possible

Don't hold one lock while trying to acquire another:

```cpp
// ❌ Risky
mtx1.lock();
// ...
mtx2.lock();  // Nested

// ✅ Better design
// Acquire all locks first
std::lock(mtx1, mtx2);
```

## Deadlock Conditions (All Must Be True)

1. **Mutual Exclusion**: Resources can't be shared
2. **Hold and Wait**: Threads hold resources while waiting for more
3. **No Preemption**: Can't forcibly take resources
4. **Circular Wait**: Circular dependency in resource requests

**To prevent deadlock**: Break ANY of these conditions.

## Timeout Pattern

Detect and recover from potential deadlock:

```cpp
auto acquired = mtx.try_lock_for(std::chrono::milliseconds(100));
if (acquired) {
    // Got lock, proceed
} else {
    // Timeout - might be deadlock, handle gracefully
    std::cout << "Couldn't acquire lock in time\n";
}
```

## Simple Rules to Remember

✅ **Always lock resources in the same order**  
✅ **Use `std::lock()` or `std::scoped_lock` for multiple locks**  
✅ **Keep critical sections small**  
✅ **Avoid nested locks**  
✅ **Don't hold locks while doing expensive operations**  
✅ **Use timeouts to detect problems early**

❌ **Never**: Acquire locks in different orders  
❌ **Never**: Hold multiple locks longer than needed  
❌ **Never**: Ignore synchronization needs

## Detection Checklist

When program hangs, check:

- [ ] Are multiple threads acquiring locks?
- [ ] Are locks acquired in different orders in different threads?
- [ ] Are there nested lock calls?
- [ ] Is a thread holding a lock while waiting for another resource?

---

**Difficulty Level**: ⭐⭐⭐ Advanced  
**Estimated Time**: 20-30 minutes  
**Prerequisites**: Mutexes, multiple locks, std::lock
