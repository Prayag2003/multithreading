# Recursive Mutexes 🔄

## The Recursion Problem

Sometimes a thread needs to lock the same mutex **multiple times**:

```cpp
#include <thread>
#include <mutex>

std::mutex mtx;

void recursiveFunction(int depth) {
    mtx.lock();  // Lock

    if (depth > 0) {
        std::cout << "Depth: " << depth << "\n";
        recursiveFunction(depth - 1);  // Call itself
        // Tries to lock again - DEADLOCK!
    }

    mtx.unlock();
}

int main() {
    recursiveFunction(3);  // Deadlock on second call!
    return 0;
}
```

**Problem**: The thread already owns the lock, but `lock()` tries to acquire it again — **deadlock!**

## The Solution: std::recursive_mutex

`std::recursive_mutex` allows the **same thread** to lock it multiple times:

```cpp
#include <thread>
#include <mutex>

std::recursive_mutex rmtx;

void recursiveFunction(int depth) {
    rmtx.lock();  // Lock (first time or reentrant)

    if (depth > 0) {
        std::cout << "Depth: " << depth << "\n";
        recursiveFunction(depth - 1);  // Lock again - OK!
    }

    rmtx.unlock();
}

int main() {
    recursiveFunction(3);  // Works fine!
    return 0;
}
```

## How It Works

```
Thread 1 - First call:        Thread 1 - Recursive call:
rmtx.lock();     ✓            rmtx.lock();     ✓ (lock count: 2)
...                           ...
recursiveFunction();          rmtx.unlock();   (lock count: 1)
...
rmtx.unlock();   (lock count: 0)
```

`std::recursive_mutex` keeps a **counter** of how many times the thread has locked it.

## Requirements for Unlocking

You must `unlock()` the same number of times you `lock()`:

```cpp
std::recursive_mutex rmtx;

rmtx.lock();      // Count: 1
rmtx.lock();      // Count: 2
rmtx.lock();      // Count: 3

rmtx.unlock();    // Count: 2
rmtx.unlock();    // Count: 1
rmtx.unlock();    // Count: 0 - Now other threads can acquire
```

**Rule**: Lock count must reach zero for other threads to acquire the mutex.

## Practical Example

```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>

std::recursive_mutex rmtx;

void printTree(const std::string& node, int depth) {
    rmtx.lock();

    std::string indent(depth * 2, ' ');
    std::cout << indent << "├─ " << node << "\n";

    if (depth < 2) {
        printTree(node + ".1", depth + 1);  // Recursive lock OK
        printTree(node + ".2", depth + 1);  // Recursive lock OK
    }

    rmtx.unlock();
}

int main() {
    std::thread t1([]() { printTree("Root", 0); });
    std::thread t2([]() { printTree("Branch", 0); });

    t1.join();
    t2.join();

    return 0;
}
```

**Output:**

```
├─ Root
  ├─ Root.1
    ├─ Root.1.1
    ├─ Root.1.2
  ├─ Root.2
    ├─ Root.2.1
    ├─ Root.2.2
├─ Branch
  ...
```

## Comparison: mutex vs recursive_mutex

```cpp
std::mutex m;
std::recursive_mutex rm;

// With std::mutex
m.lock();
m.lock();   // ❌ DEADLOCK!
m.unlock();
m.unlock();

// With std::recursive_mutex
rm.lock();
rm.lock();  // ✅ OK, counter increments
rm.unlock();
rm.unlock();
```

| Feature         | `std::mutex` | `std::recursive_mutex` |
| --------------- | ------------ | ---------------------- |
| **Reentrant**   | No           | Yes                    |
| **Lock count**  | N/A          | Tracked                |
| **Performance** | Faster       | Slightly slower        |
| **Use case**    | Simple locks | Recursive code         |

## When to Use Recursive Mutex

✅ **Use when:**

- Function calls itself recursively
- Multiple overloads lock and call each other
- Tree/graph traversal with shared synchronization

❌ **Avoid when:**

- You can redesign to avoid recursion
- Performance is critical
- Simple sequential code

## Anti-Pattern: Hiding Recursion

```cpp
// ❌ BAD DESIGN - Recursive mutex hiding poor design
std::recursive_mutex rmtx;

void publicAPI() {
    rmtx.lock();
    internalHelper();
    rmtx.unlock();
}

void internalHelper() {
    rmtx.lock();  // Why lock again?
    // Do work
    rmtx.unlock();
}

// ✅ BETTER DESIGN - Split locked and unlocked versions
std::mutex mtx;

void publicAPI() {
    mtx.lock();
    internalHelperUnlocked();
    mtx.unlock();
}

void internalHelperUnlocked() {
    // Do work (assumes caller holds lock)
}
```

**Better design**: Create separate functions for locked and unlocked operations.

## Real Example: Graph Traversal

```cpp
#include <thread>
#include <mutex>
#include <iostream>

struct Node {
    int value;
    std::vector<Node*> children;
};

std::recursive_mutex graphMtx;

void traverseGraph(Node* node) {
    if (!node) return;

    graphMtx.lock();
    std::cout << "Visiting: " << node->value << "\n";
    graphMtx.unlock();

    for (auto child : node->children) {
        traverseGraph(child);  // Recursive call with lock/unlock
    }
}

int main() {
    Node root{1};
    Node child1{2}, child2{3};
    root.children = {&child1, &child2};

    std::thread t(traverseGraph, &root);
    t.join();

    return 0;
}
```

## Performance Considerations

```
std::mutex:           ║ std::recursive_mutex:
lock() → acquire      ║ lock() → check lock count
unlock() → release    ║           increment/decrement
                      ║ unlock() → decrement
                      ║           release if count=0
                      ║
Simple, fast          ║ Slightly more overhead
```

**Estimate**: 10-20% performance penalty for recursive mutex.

## Key Takeaways

✅ `std::recursive_mutex` allows same thread to lock multiple times  
✅ Lock count must be balanced with unlock count  
✅ Useful for recursive functions  
✅ Use with lock guards for exception safety  
✅ Consider redesigning to avoid recursion when possible  
✅ Small performance cost compared to regular mutex

## Common Mistakes ⚠️

### Mistake 1: Unbalanced Lock/Unlock

```cpp
// ❌ WRONG
rmtx.lock();
rmtx.lock();
rmtx.unlock();
// Lock count still 1, mutex never fully released!

// ✅ CORRECT
rmtx.lock();
rmtx.lock();
rmtx.unlock();
rmtx.unlock();
```

### Mistake 2: Different Threads

```cpp
// ❌ WRONG
std::recursive_mutex rmtx;
rmtx.lock();           // Thread A owns it

// In Thread B:
rmtx.lock();  // Still blocks if Thread A holds it!
```

Recursive mutex only helps the **same thread**. Other threads still block.

## Next Steps

- Learn about lock guards (`07_lock_guard.cpp`)
- Explore condition variables (`09_conditional_variable.cpp`)

---

**Difficulty Level**: ⭐⭐ Intermediate  
**Estimated Time**: 20-30 minutes  
**Prerequisites**: Mutex basics, recursion, threads
