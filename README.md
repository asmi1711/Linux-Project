# Linux Systems Programming Project
7th Semester project for LINUX CS4154
#Thread-Safe Producer-Consumer with Priority Producer

This project implements the classic Producer–Consumer problem in C using POSIX threads.
It extends the standard model by adding a priority producer that can insert items at the front of the shared queue, interrupting the normal flow of producers and consumers.

The program demonstrates thread synchronization, mutual exclusion, and efficient coordination between threads using condition variables.

#Features:
1)Multiple producers and consumers sharing a bounded circular buffer
2)Thread-safe synchronization using pthread_mutex and pthread_cond
3)Priority producer capable of queue preemption
4)Blocking behavior when the buffer is full or empty
5)Adjustable constants for buffer size, item count, and thread count

#How to Build and Run:
gcc -pthread -o priority_producer_consumer priority_producer_consumer.c
./priority_producer_consumer

#System Requirements
1)Linux environment
2)GCC compiler with pthread support

This program was developed as part of a Linux systems programming course to demonstrate concurrency, synchronization, and priority-based task handling using POSIX threads.
