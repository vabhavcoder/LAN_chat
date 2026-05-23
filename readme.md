# Cpp LAN Chat 🚀

A lightweight, multi-threaded local area network (LAN) chat application built entirely from scratch in C++. 

This project demonstrates a deep understanding of low-level networking and concurrency by bypassing high-level libraries to use raw TCP/IP sockets and manual thread management.

## ✨ Features
* **Raw TCP Sockets:** Custom-built server and client architecture using `<sys/socket.h>`.
* **Multi-threading:** Utilizes `std::thread` to handle multiple clients simultaneously without blocking I/O.
* **Thread Safety:** Implements `std::mutex` to prevent race conditions during client connection and disconnection.
* **Real-Time Broadcasting:** Messages are instantly pushed to all connected clients.
* **Clean Terminal UI:** Uses carriage returns (`\r`) to maintain a clean chat interface.

## 🛠️ Prerequisites
To build and run this project, you need:
* A Linux/macOS environment (Windows requires Winsock adjustments).
* **C++17** or higher.
* **CMake** (version 3.10+).
* **Make** or another build tool.

## ⚙️ Build Instructions

Clone the repository and compile the project using CMake:

```bash
git clone [https://github.com/YOUR_USERNAME/Cpp-LAN-Chat.git](https://github.com/YOUR_USERNAME/Cpp-LAN-Chat.git)
cd Cpp-LAN-Chat
mkdir build
cd build
cmake ..
make
