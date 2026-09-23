# Kernel Monitoring System

## Project Overview

Kernel Monitoring System is a Linux-based system programming project developed using C. The project monitors system and process-related information using Linux system interfaces and the `/proc` filesystem.

The system displays information about CPU, memory, and running processes. It also monitors process ID, process state, memory usage, CPU usage, and identifies processes that use higher system resources.

## Features

- Displays CPU information
- Displays total and available memory
- Displays running processes
- Displays Process ID (PID)
- Displays process name
- Displays process state
- Displays process memory usage
- Calculates process CPU usage
- Identifies high CPU usage
- Identifies high memory usage
- Continuously refreshes monitoring information
- Uses Linux `/proc` filesystem
- Runs completely through the Ubuntu terminal

## Technologies Used

- C Programming
- Linux / Ubuntu
- GCC Compiler
- Linux `/proc` Filesystem
- POSIX APIs
- Git and GitHub

## Project Structure

```text
KernelMonitoring/
├── bin/
├── docs/
├── include/
└── src/
    ├── cpu_monitor.c
    ├── memory_monitor.c
    ├── process_monitor.c
    ├── cpu_process_test.c
    ├── kernel_monitor.c
    ├── kernel_monitor_backup.c
    ├── kernel_monitor_cpu_backup.c
    └── kernel_monitor_resource_backup.c
