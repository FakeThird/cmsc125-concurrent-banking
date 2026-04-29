# About `concurrent-banking`

# Creator

- Keith Ashly Domingo (TheAshly/FakeThird)
- John Clyde Aparicio (Cl4-Bisk)

# Description

**`concurrent-banking`** is currently an assignment focusing on concurrency control in a multi-threaded banking system using real POSIX threads (pthreads) and synchronization primitives. The goal is to build an in-memory bank that processes concurrent transactions, implements proper isolation through locking, manages a bounded buffer pool with semaphores, and handles deadlock through either prevention or detection in C and through this exercise gain hands-on experience with the classical concurrency problems that real systems confront every day.
```
Created as a laboratory assignment for CMSC 125 Operating Systems.
```

# Features and Algorithms

**`concurrent-banking`** offers certain features, which follows as:

- **Multi-threaded transaction execution** — each transaction runs in its own `pthread`, spawned by `main.c` after parsing the trace file. Transactions wait for their scheduled start tick before executing.
- **Reader-writer lock isolation** — account reads (`BALANCE`) use `pthread_rwlock_rdlock` allowing multiple concurrent readers, while writes (`DEPOSIT`, `WITHDRAW`, `TRANSFER`) use `pthread_rwlock_wrlock` for exclusive access.
- **Deadlock prevention via lock ordering** — the `transfer` function in `bank.c` always acquires account locks in ascending order of account ID, breaking the circular wait Coffman condition.
- **Bounded buffer pool** — a fixed pool of 5 slots coordinated by POSIX semaphores (`sem_t`) implements the producer-consumer pattern. `load_account` acts as producer and `unload_account` as consumer.
- **Simulated discrete clock** — a dedicated timer thread increments `global_tick` every `--tick-ms` milliseconds and broadcasts on a condition variable to wake waiting transaction threads.
- **Trace file parser** — `load_transactions` in `transaction.c` reads a structured trace file and populates `Transaction` structs with their scheduled operations (`DEPOSIT`, `WITHDRAW`, `TRANSFER`, `BALANCE`).
- **Account file parser** — `load_accounts` in `bank.c` reads initial account balances from a file into the in-memory `Bank` struct.
- **Balance conservation output** — after all transactions complete, `print_accounts_to_file` writes the final account balances to `tests/post_accounts.txt` in the same format as the input file.

# Compilation and Usage

**Requirements:** GCC with POSIX thread support (`-pthread`), a Linux or macOS environment.

**Compile (optimized):**
```bash
make all
```

**Compile (with ThreadSanitizer for race detection):**
```bash
make debug
```

**Clean build artifacts:**
```bash
make clean
```

**Run manually:**
```bash
./bankdb --accounts=tests/accounts.txt --trace=tests/trace_simple.txt --deadlock=prevention --tick-ms=100
```

**Available flags:**

| Flag | Description | Default |
|---|---|---|
| `--accounts=FILE` | Path to initial account balances file | _(required)_ |
| `--trace=FILE` | Path to transaction trace file | _(required)_ |
| `--deadlock=prevention\|detection` | Deadlock handling strategy | `detection` |
| `--tick-ms=N` | Milliseconds per simulation tick | `100` |
| `--verbose` | Print detailed operation logs | off |

# Example Test Usage

**Run all tests:**
```bash
make test
```

**Test 1 — No conflicts (sequential operations on one account):**
```bash
make test1
```

**Test 2 — Concurrent readers (multiple BALANCE ops at the same tick):**
```bash
make test2
```

**Test 3 — Deadlock scenario with prevention strategy:**
```bash
make test3_prevention
```

**Test 3 — Deadlock scenario with detection strategy:**
```bash
make test3_detection
```

**Test 4 — Insufficient funds abort:**
```bash
make test4
```

**Test 5 — Buffer pool saturation (6 transactions, pool size 5):**
```bash
make test5
```

After any run, final account balances are written to `tests/post_accounts.txt`.

# Limitations and Bugs

### 1. Per-account `pthread_rwlock_t` is not explicitly initialized
The rwlock inside each `Account` struct is not initialized with `pthread_rwlock_init` before use. It relies on zero-initialization of the global `Bank` struct, which is not guaranteed to be a valid lock state on all platforms.

# Design and Architecture Overview

The system is organized in layers. `main.c` sits at the top — it parses arguments, loads accounts and transactions, initializes the bank, starts the timer thread, and spawns one pthread per transaction. Below it, `transaction.c` contains the thread function `execute_transaction` which waits for the correct tick then dispatches operations to `bank.c`. `bank.c` owns all account data and the four banking operations, each protected by per-account `pthread_rwlock_t`. `timer.c` runs a dedicated thread that increments `global_tick` on a fixed interval and broadcasts on a condition variable to wake waiting transactions. `buffer_pool.c` implements the bounded buffer using POSIX semaphores. `lock_manager.c` contains the deadlock detection infrastructure. `utils.c` provides `find_account` which searches the bank by account ID. `metrics.c` handles final output reporting.

# License

**Educational Use Only**

This project was created as an laboratory project for the CMSC 125 - Operating Systems.

- Free for **educational purposes** (learning, teaching, academic research)
- Free for **personal, non-commercial use**

**Copyright © 2026 Keith Ashly M. Domingo and John Clyde C. Aparicio**
