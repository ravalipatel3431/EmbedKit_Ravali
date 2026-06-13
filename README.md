# EmbedKit Ravali

Name: Ravali

Module: Ring Buffer (Circular Buffer)

Description:
This project implements an 8-byte Ring Buffer using C. The buffer supports initialization, write, read, full check, empty check, and count operations. It demonstrates circular wrap-around using bitwise AND optimization.

Features:

* Initialize buffer
* Write one byte
* Read one byte
* Check if buffer is full
* Check if buffer is empty
* Get current byte count
* Circular wrap-around using &(BUFFER_SIZE - 1)

Build Command:
gcc -Wall -std=c99 ringbuf.c -o ringbuf

Run Command:
./ringbuf

Expected Output:
Demonstrates successful writes, reads, buffer full condition, buffer empty condition, and wrap-around behavior.
