A real-time operating system (RTOS) project demonstrating multi-tasking, inter-process communication, and synchronization on ARM Cortex-M3 architecture using FreeRTOS and QEMU emulation.

This project implements a FreeRTOS-based embedded system with multiple concurrent tasks that communicate through queues and synchronize access to shared resources using mutexes. The application runs on an emulated ARM Cortex-M3 processor using QEMU.

 Features
Multi-tasking: Three concurrent tasks with different priorities
Task Synchronization: Mutex-based protection for shared UART resource
Inter-Task Communication: Queue-based message passing between tasks
Real-Time Scheduling: Preemptive priority-based task scheduling
Hardware Emulation: Runs on QEMU without physical hardware

Task Description
Task 1 (LED1): Simulates LED toggling every 500ms, sends status updates via queue
Task 2 (LED2): Simulates LED toggling every 1000ms, sends status updates via queue
Monitor Task: Receives messages from other tasks and displays them via UART
