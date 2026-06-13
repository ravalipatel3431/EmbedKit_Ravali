# EmbedKit_Ravali

**Name:** B. Ravali

## Module

Ring Buffer (Circular Buffer) Implementation in C

## Description

This project implements an 8-byte Ring Buffer using C. The buffer supports initialization, write, read, full check, empty check, and count operations. The implementation demonstrates circular wrap-around using bitwise AND optimization instead of modulo division.

## Features

* Initialize Ring Buffer
* Write one byte into the buffer
* Read one byte from the buffer
* Check if buffer is full
* Check if buffer is empty
* Get current number of stored bytes
* Circular wrap-around using `& (BUFFER_SIZE - 1)`
* Buffer overflow protection
* Buffer underflow protection

## Optimization

The implementation uses:

```c
head = (head + 1U) & (BUFFER_SIZE - 1U);
tail = (tail + 1U) & (BUFFER_SIZE - 1U);
```

instead of modulo (`%`) for index wrap-around.

### Advantages

* Faster on MCUs without a hardware divider.
* Avoids costly division operations.
* Efficient for embedded systems.
* Works only when `BUFFER_SIZE` is a power of 2.

## Buffer Configuration

```c
#define BUFFER_SIZE (8U)
```

The buffer size must be a power of 2 for the bitwise wrap-around technique to work correctly.

## Build Command

```bash
gcc -Wall -std=c99 ringbuf.c -o ringbuf
```

## Run Command

### Linux

```bash
./ringbuf
```

### Windows

```bash
ringbuf.exe
```

## Program Flow

### Step 1

Fill the buffer with 8 bytes and verify the full condition.

### Step 2

Attempt to write one additional byte and verify overflow protection.

### Step 3

Read three bytes from the buffer.

### Step 4

Write three new bytes into the freed locations and verify circular wrap-around.

### Step 5

Read all remaining bytes and verify FIFO behavior.

### Step 6

Attempt to read from an empty buffer and verify underflow protection.

## Expected Output

***** STEP 1 *****
[WRITE] 0x41 -> OK (count=1)
[WRITE] 0x42 -> OK (count=2)
[WRITE] 0x43 -> OK (count=3)
[WRITE] 0x44 -> OK (count=4)
[WRITE] 0x45 -> OK (count=5)
[WRITE] 0x46 -> OK (count=6)
[WRITE] 0x47 -> OK (count=7)
[WRITE] 0x48 -> OK (count=8) FULL
Buffer Full = YES

***** STEP 2 *****
[WRITE] 0x99 -> FAIL (buffer full)

***** STEP 3 *****
[READ] -> 0x41 (count=7)
[READ] -> 0x42 (count=6)
[READ] -> 0x43 (count=5)

***** STEP 4 *****
[WRITE] 0x49 -> OK (count=6)
[WRITE] 0x4A -> OK (count=7)
[WRITE] 0x4B -> OK (count=8)
Buffer Full = YES

***** STEP 5 *****
[READ] -> 0x44 (count=7)
[READ] -> 0x45 (count=6)
[READ] -> 0x46 (count=5)
[READ] -> 0x47 (count=4)
[READ] -> 0x48 (count=3)
[READ] -> 0x49 (count=2)
[READ] -> 0x4A (count=1)
[READ] -> 0x4B (count=0)
Buffer Empty = YES

***** STEP 6 *****
[READ] (empty) -> FAIL (buffer empty)

## Concepts Demonstrated

* Ring Buffer (Circular Buffer)
* FIFO Data Structure
* Embedded C Programming
* Buffer Overflow Protection
* Buffer Underflow Protection
* Bitwise Optimization
* Memory-Efficient Data Handling
* Circular Queue Implementation

## Author

B.Ravali


