# Design Documentation

## 1. Deadlock Strategy Choice

**Chosen Strategy:** Prevention

**Reason for choice:**
We chose deadlock prevention over detection because it provides a simpler and more predictable approach for this banking system. Prevention eliminates the possibility of deadlocks entirely through lock ordering, avoiding the complexity and overhead of runtime deadlock detection and recovery mechanisms. This is particularly suitable for a transaction processing system where predictability and avoiding transaction aborts is crucial.

**How it works in our implementation:**
The transfer() function in bank.c implements lock ordering by acquiring locks in ascending order of account IDs. When transferring from account A to account B, it determines which account has the lower ID and acquires that lock first, then the higher ID lock second. This ensures that all transactions follow a consistent global ordering, preventing circular wait conditions.

**Coffman condition broken:**
This approach breaks the "circular wait" condition of the Coffman conditions. By enforcing a total ordering on lock acquisition (by account ID), we ensure that no cycle can form in the resource allocation graph, as locks are always acquired in a predetermined sequence regardless of the transaction's direction.

(Arpaci-Dusseau & Arpaci-Dusseau, 2018, Chapter 32)

---

## 2. Buffer Pool Integration

**When accounts are loaded:**
The load_account() function is called as a producer operation when a transaction needs to access an account that isn't currently in the buffer pool. It waits for an empty slot using sem_wait on empty_slots, then loads the account data into an available buffer slot.

**When accounts are unloaded:**
The unload_account() function serves as the consumer operation, called when a transaction completes and the account can be evicted from the buffer pool. It waits for a full slot using sem_wait on full_slots, then marks the specified account slot as unused.

**What happens when the pool is full:**
When the buffer pool is full, sem_wait on empty_slots blocks the load_account() operation until an unload_account() call signals that a slot has been freed by posting to empty_slots. This implements the buffer problem using semaphores to coordinate between producers and consumers.

**Justification:**
We chose this semaphore-based producer-consumer pattern because it provides clean synchronization for the buffer problem. The semaphores naturally handle the blocking behavior when the pool is full or empty, and the mutex protects the critical section where buffer slots are manipulated. This approach is more efficient than busy-waiting alternatives and ensures proper coordination between concurrent load/unload operations.

(Arpaci-Dusseau & Arpaci-Dusseau, 2018, Chapter 31)

---

## 3. Reader-Writer Lock Performance

**Benchmark setup:**
We used the trace_readers.txt trace file which contains multiple concurrent balance operations on the same account. We measured the total ticks required to complete all transactions using both pthread_mutex_t and pthread_rwlock_t configurations.

| Configuration    | Ticks to complete |
| ---------------- | ----------------- |
| pthread_mutex_t  | 1 tick            |
| pthread_rwlock_t | 1 tick            |

**Observation:**
Both configurations completed in 1 tick, but the rwlock version allows concurrent reads while the mutex version would serialize all operations. The performance difference becomes apparent with more complex workloads involving mixed read/write operations.

**Why rwlock helps on read-heavy workloads:**
Reader-writer locks allow multiple concurrent readers to access the same account simultaneously for balance checks, while still providing exclusive access for write operations like deposits and withdrawals. This improves throughput for read-heavy workloads by eliminating unnecessary serialization of read-only operations.

(Arpaci-Dusseau & Arpaci-Dusseau, 2018, Chapter 29)

---

## 4. Timer Thread Design

**Purpose of the timer thread:**
The timer thread maintains a global discrete clock that increments at fixed intervals, coordinating the execution timing of concurrent transactions. It uses a condition variable to wake up transactions waiting for their scheduled start time, enabling deterministic testing of concurrent scenarios.

**What would break without it:**
Without the timer thread, transactions would execute immediately upon thread creation, making it impossible to control the interleaving of operations for testing purposes. Race conditions would be unpredictable, and it would be difficult to reproduce specific concurrency scenarios or measure timing-dependent behaviors.

**How it enables true concurrency testing:**
The start_tick and wait_until_tick functions work together to implement a discrete event simulation. Transactions specify their start time in ticks, and wait_until_tick blocks until the global clock reaches that tick. This allows precise control over when transactions begin, enabling systematic testing of different interleavings and timing-dependent concurrency issues.

(Arpaci-Dusseau & Arpaci-Dusseau, 2018, Chapters 30 & 26)

## References

[Arpaci-Dusseau, R. H., & Arpaci-Dusseau, A. C. (2018). _Operating systems: Three easy pieces_. Arpaci-Dusseau Books.](https://pages.cs.wisc.edu/~remzi/OSTEP/)