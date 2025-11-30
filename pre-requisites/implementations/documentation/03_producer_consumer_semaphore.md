# Producer-Consumer with Semaphore 🔄

## Overview

This example shows the producer-consumer pattern using binary semaphores instead of mutexes and condition variables. It's simpler and more elegant for this use case.

## Understanding the Code

```cpp
#define size 5
int buffer[size];

std::binary_semaphore signal_to_producer{1};   // Producer can start
std::binary_semaphore signal_to_consumer{0};   // Consumer waits

void produce() {
    while (1) {
        signal_to_producer.acquire();  // Wait for permission
        cout << "Produced: ";
        for (int i = 0; i < size; i++) {
            buffer[i] = pow((i + 1), 3);
            cout << buffer[i] << " ";
        }
        cout << "\n";
        signal_to_consumer.release();  // Signal consumer
    }
}

void consume() {
    while (1) {
        signal_to_consumer.acquire();  // Wait for producer
        cout << "Consumed: ";
        for (int i = size - 1; i >= 0; i--) {
            cout << buffer[i] << " ";
            buffer[i] = 0;
        }
        cout << "\n\n";
        signal_to_producer.release();  // Signal producer
    }
}
```

## How It Works

### Initial State
- `signal_to_producer = 1`: Producer can start immediately
- `signal_to_consumer = 0`: Consumer waits

### Producer Flow
1. Acquires `signal_to_producer` (waits if 0)
2. Fills buffer with data
3. Releases `signal_to_consumer` (signals consumer)

### Consumer Flow
1. Acquires `signal_to_consumer` (waits if 0)
2. Reads and clears buffer
3. Releases `signal_to_producer` (signals producer)

## Key Points

**Semaphore coordination:**
- Producer waits for consumer to finish
- Consumer waits for producer to finish
- They take turns using the buffer

**No mutex needed:**
- Semaphores handle synchronization
- Simpler than mutex + condition variable
- But only works for this specific pattern

## Comparison with Mutex Version

**Mutex + Condition Variable:**
- More flexible
- Can check buffer size
- More complex code

**Semaphore:**
- Simpler code
- Fixed buffer size
- Less flexible

## Important Notes

⚠️ **Fixed buffer size** - Size must be known at compile time  
⚠️ **C++20 required** - Binary semaphore is C++20 feature  
⚠️ **No size checking** - Assumes buffer is always full/empty  
⚠️ **Infinite loop** - Runs forever (add exit condition for real use)

## When to Use

**Use semaphore version when:**
- Fixed buffer size
- Simple producer-consumer
- Want simpler code

**Use mutex version when:**
- Dynamic buffer size
- Need to check buffer state
- More complex coordination needed

## Next Steps

- Review all implementations to understand different approaches
