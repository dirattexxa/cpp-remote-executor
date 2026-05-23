# Lightweight Cross-Platform Remote Administration Tool

This is an educational, lightweight implementation of a Remote Administration Tool (RAT) architecture, consisting of a Command and Control (C2) server and background agents. The project was built to explore network programming, low-level operating system APIs, and cross-platform compilation techniques.

The repository contains distinct native implementations for both **Linux (POSIX Sockets)** and **Windows (Winsock2)** environments.

---

## 🎯 Purpose

The primary goal of this project was to study and verify concepts in systems programming and network security:
- **Network I/O:** Handling raw TCP sockets, data streaming, and connection resilience.
- **OS Subsystems:** Managing background execution via daemonization on Linux and native window hiding (`ShowWindow`) on Windows.
- **Inter-Process Communication:** Executing shell commands through pipes (`popen`/`_popen`) and capturing standard output.

---

## 📂 Repository Structure

- `server.cpp` — The C2 server module (runs on Linux, listens for incoming agent connections, and provides an interactive shell interface).
- `/agent` — Target machine modules:
  - `agent_linux.cpp` — Native Linux agent using POSIX API and background daemon logic.
  - `agent_windows.cpp` — Native Windows agent using Winsock2 and hidden execution routines.

---

## 🚀 How to Use

### 1. Compilation

**Building the Linux Server & Agent:**
```bash
g++ server.cpp -o server
g++ agent_linux.cpp -o agent_linux
