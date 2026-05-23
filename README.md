# Lightweight Remote Administration Tool (RAT) 🚀

A lightweight, educational Cross-Platform Remote Administration Tool written in C++ for learning network programming, low-level OS APIs, and security concepts. 

The project features a central C2 (Command and Control) Server and modular agents capable of running in the background on both **Linux** and **Windows**.

---

## ⚡ Features

- **Cross-Platform Agents:** Native implementations for both Linux (POSIX) and Windows (Winsock2).
- **Stealth Mode (Windows):** Instantly hides the console window on startup using native WinAPI (`ShowWindow`).
- **Daemonization (Linux):** Detaches the process from the controlling terminal to run silently in the background.
- **Auto-Reconnection:** Intelligent retry logic that attempts to re-establish connection every few seconds if the server goes offline.
- **Remote Shell Exec:** Executes incoming shell commands on the target machine via pipes (`_popen`/`popen`) and streams the output back to the controller.

---

## 📂 Project Structure

- `/server` — Linux-based C2 Server written in C++ (manages incoming connections and sends commands).
- `/agent` — Source code for the target machine modules:
  - `main.cpp` (or `agent_windows.cpp`) — Windows API / Winsock2 implementation.
  - `agent_linux.cpp` — Linux POSIX sockets implementation.

---

## 🛠️ Compilation Guide

### Compiling for Windows (Cross-Compilation from Arch Linux)
To build the Windows executable directly from your Arch Linux machine using `mingw-w64`:
```bash
x86_64-w64-mingw32-g++ main.cpp -o agent.exe -lws2_32
