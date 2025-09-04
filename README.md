# Concurrent Linked List Lab

This repository contains implementations of a concurrent linked list lab using **serial**, **mutex**, and **reader-writer lock (rwlock)** approaches. It also includes a shell script to run all experiments and a Python script to generate graphs from the results.

The purpose of this lab is to compare the performance of different synchronization mechanisms under varying operation mixes and thread counts.

---

## Repository Structure

| File | Description |
|------|-------------|
| `serial.c` | Serial (single-threaded) implementation of the linked list. Performs `Member`, `Insert`, and `Delete` operations sequentially. |
| `mutex.c`  | Multi-threaded linked list implementation using **mutex locks** to synchronize access. |
| `rwlock.c` | Multi-threaded linked list implementation using **reader-writer locks (pthread_rwlock_t)** for concurrent reads and exclusive writes. |
| `run_all.sh` | Bash script to run all experiments for multiple cases and thread counts, and collect results in a CSV file (`results.csv`). |
| `graphs.py` | Python script to visualize performance results from `results.csv`. Generates plots showing time vs. number of threads for different synchronization mechanisms. |

---

## Prerequisites

1. **Linux / WSL / macOS**  
2. **GCC** (with pthread support)  
   ```bash
   gcc --version
   ````

3. **Python 3** (with `matplotlib` and `pandas`)

   ```bash
   pip install matplotlib pandas
   ```

---

## Compilation Instructions

Compile each C program using GCC with `-pthread`:

```bash
gcc -O2 -std=c11 serial.c -o serial
gcc -O2 -std=c11 mutex.c -o mutex -pthread
gcc -O2 -std=c11 rwlock.c -o rwlock -pthread
```

> **Note:** Mutex and RWLock versions require pthread support.

---

## Running the Experiments

A bash script `run_all.sh` is provided to run all test cases automatically.

1. Make the script executable:

```bash
chmod +x run_all.sh
```

2. Run the script:

```bash
./run_all.sh
```

3. This will generate a `results.csv` file in the same directory. The CSV columns are:

```
case,implementation,threads,rep,time_us,seed
```

* `case` → Case ID (1, 2, or 3)
* `implementation` → `serial`, `mutex`, or `rwlock`
* `threads` → Number of threads used
* `rep` → Repetition index (0-9)
* `time_us` → Elapsed time in microseconds
* `seed` → Random seed used for reproducibility

---

## Cases

The experiments are run on three predefined cases:

| Case | Member Fraction | Insert Fraction | Delete Fraction |
| ---- | --------------- | --------------- | --------------- |
| 1    | 0.99            | 0.005           | 0.005           |
| 2    | 0.90            | 0.05            | 0.05            |
| 3    | 0.50            | 0.25            | 0.25            |

These cases simulate workloads with varying read/write contention.

---

## Running the Graphs

After running `run_all.sh`, visualize the results with:

```bash
python3 graphs.py results.csv
```

This script will generate plots showing execution time versus number of threads for each implementation and case.

---

## Notes

* Serial version always runs with 1 thread.
* Mutex version serializes all operations, which may limit scalability.
* RWLock version allows multiple concurrent `Member` operations but blocks for writes.
* Each experiment is repeated 10 times to account for variability.

---

## Example Usage

```bash
# Compile all programs
gcc -O2 -std=c11 serial.c -o serial
gcc -O2 -std=c11 mutex.c -o mutex -pthread
gcc -O2 -std=c11 rwlock.c -o rwlock -pthread

# Run all cases
./run_all.sh

# Generate graphs
python3 graphs.py results.csv
```

---

## License

This repository is for educational purposes. Feel free to use and modify the code for learning and experimentation.

```
