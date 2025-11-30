# Multiple Locks: Preventing Deadlock 🔐

## The Deadlock Problem

What happens when a thread needs to lock multiple resources?

```cpp
std::mutex mtx1, mtx2;

// Thread A                          Thread B
mtx1.lock();     // Acquire mtx1    mtx2.lock();     // Acquire mtx2
// ... work ...
mtx2.lock();     // Wait for mtx2   mtx1.lock();     // Wait for mtx1
// Both threads wait forever - DEADLOCK!
```

## The Classic Deadlock Scenario

**Thread A wants resources in order: A → B**  
**Thread B wants resources in order: B → A**

```
Thread A                    Thread B
Lock A ✓                    Lock B ✓
Waiting for B...            Waiting for A...
  ↓                           ↓
   DEADLOCK (both waiting)
```

## Solution 1: Consistent Lock Ordering

Always acquire locks in the **same order** everywhere:

```cpp
std::mutex mtx1, mtx2;

void threadAFunction() {
    mtx1.lock();
    // ... work with resource 1 ...
    mtx2.lock();
    // ... work with both resources ...
    mtx2.unlock();
    mtx1.unlock();
}

void threadBFunction() {
    mtx1.lock();  // Always lock mtx1 first, just like threadA
    // ... work with resource 1 ...
    mtx2.lock();
    // ... work with both resources ...
    mtx2.unlock();
    mtx1.unlock();
}
```

**Key Rule**: If you need multiple locks, always acquire them in the **same order** in all threads.

## Solution 2: std::lock() Function

`std::lock()` acquires multiple locks safely without deadlock:

```cpp
#include <thread>
#include <mutex>
#include <iostream>

std::mutex mtx1, mtx2;
int account1 = 100, account2 = 200;

void transferMoney() {
    std::lock(mtx1, mtx2);  // Safely lock both without deadlock risk

    account1 -= 10;
    account2 += 10;

    mtx1.unlock();
    mtx2.unlock();
}

int main() {
    std::thread t1(transferMoney);
    std::thread t2(transferMoney);

    t1.join();
    t2.join();

    return 0;
}
```

**How it works**: `std::lock()` detects potential deadlocks and acquires locks in a deadlock-free manner.

## Visual Comparison

### Naive Approach (Deadlock Risk)

```
Thread 1:  Lock A → Lock B
Thread 2:  Lock B → Lock A
Result: DEADLOCK ⚠️
```

### Consistent Ordering (Safe)

```
Thread 1:  Lock A → Lock B
Thread 2:  Lock A → Lock B  (same order!)
Result: No deadlock ✅
```

### std::lock (Safe & Automatic)

```
Thread 1:  std::lock(A, B)  (handles ordering)
Thread 2:  std::lock(A, B)  (handles ordering)
Result: No deadlock ✅
```

## Practical Example: Bank Transfer

```cpp
#include <thread>
#include <mutex>
#include <iostream>

class Account {
    double balance = 0;
    std::mutex mtx;

public:
    Account(double initialBalance) : balance(initialBalance) {}

    // Safe transfer using std::lock
    void transferTo(Account& other, double amount) {
        std::lock(mtx, other.mtx);  // Lock both atomically

        if (balance >= amount) {
            balance -= amount;
            other.balance += amount;
            std::cout << "Transfer successful\n";
        } else {
            std::cout << "Insufficient funds\n";
        }

        mtx.unlock();
        other.mtx.unlock();
    }

    double getBalance() const {
        return balance;
    }
};

int main() {
    Account alice(1000);
    Account bob(500);

    std::thread t1([&]() { alice.transferTo(bob, 100); });
    std::thread t2([&]() { bob.transferTo(alice, 50); });

    t1.join();
    t2.join();

    std::cout << "Alice: " << alice.getBalance() << "\n";  // 950
    std::cout << "Bob: " << bob.getBalance() << "\n";      // 550

    return 0;
}
```

## std::lock with Lock Guards (Most Modern Way)

```cpp
#include <thread>
#include <mutex>

std::mutex mtx1, mtx2;

void modernApproach() {
    std::lock(mtx1, mtx2);  // Acquire both atomically

    std::lock_guard<std::mutex> lg1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lg2(mtx2, std::adopt_lock);

    // Automatic unlock via lock_guard destructors
    // Critical section here
}
```

**`std::adopt_lock`**: Tells lock_guard we already own the lock.

## Deadlock Detection Checklist

❌ **Deadlock likely if:**

- Multiple threads lock multiple mutexes
- Locking order differs between threads
- Holding a lock while waiting for another
- Nested locks in different orders

✅ **Prevent deadlock by:**

- Using consistent lock ordering across all threads
- Using `std::lock()` to acquire multiple locks
- Keeping critical sections small
- Using lock guards for automatic unlock
- Avoiding nested locks if possible

## When to Use Each Approach

| Approach            | When to Use    | Pros              | Cons              |
| ------------------- | -------------- | ----------------- | ----------------- |
| Consistent ordering | Simple cases   | Predictable       | Manual discipline |
| `std::lock()`       | Multiple locks | Deadlock-safe     | Slightly complex  |
| Lock guards         | All cases      | Automatic cleanup | Requires C++17    |

## Common Patterns

### Pattern 1: Two Mutex Transfer

```cpp
std::lock(mtx1, mtx2);
std::lock_guard<std::mutex> lg1(mtx1, std::adopt_lock);
std::lock_guard<std::mutex> lg2(mtx2, std::adopt_lock);
```

### Pattern 2: Always Lock in Order

```cpp
// Always lock in ascending order
mtx_A.lock();
mtx_B.lock();  // Always B after A
mtx_C.lock();  // Always C after B
```

### Pattern 3: Scoped Lock (C++17)

```cpp
#include <thread>
#include <mutex>

std::mutex m1, m2;

void cppSeventeen() {
    std::scoped_lock lock(m1, m2);  // Automatic, safe
    // Critical section
}  // Automatic unlock
```

## Key Takeaways

✅ Deadlock happens when threads wait for locks in circular patterns  
✅ Use consistent lock ordering across all threads  
✅ Use `std::lock()` to safely acquire multiple locks  
✅ Modern C++: Use `std::scoped_lock`  
✅ Always use lock guards for automatic cleanup  
✅ Keep critical sections small

## Next Steps

- Learn about `std::lock_guard` (`07_lock_guard.cpp`)
- Explore condition variables (`09_conditional_variable.cpp`)
