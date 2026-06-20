# 🍅 Tomato - Enterprise Food Delivery Backend Simulation

A high-performance, strictly-typed C++ backend architecture simulating the core orchestration of a modern food delivery platform (e.g., Zomato, Swiggy, UberEats). 

This project goes beyond simple scripts, implementing a **Multi-Role State Machine** with dedicated interactive portals, dynamic distance-based pricing, and production-grade exception handling.

## 🚀 Core Architecture

* **Role-Based Portals:** Synchronized interactive dashboards for **Customers**, **Delivery Partners**, and **Restaurant Admins**.
* **State-Driven Orchestration:** Lifecycle tracking of global orders (`Pending` ➔ `Preparing` ➔ `Out for Delivery` ➔ `Delivered`).
* **Dynamic Distance Pricing:** Calculates delivery fees using Pythagorean coordinate geometry mapping (Target X/Y vs. Current X/Y).
* **Enterprise Error Handling:** Replaces boolean checks with custom `std::runtime_error` classes (`InsufficientFundsException`, `InvalidMenuException`).
* **Simulated Real-Time UX:** Implements `<thread>` and `<chrono>` to replicate network API latency and asynchronous task loading.

## 🛠 Tech Stack & OOP Concepts

* **Language:** C++14/C++17
* **Data Structures:** `<vector>`, `<map>` (for O(log n) cart aggregation).
* **OOP Pillars Implemented:**
  * **Inheritance:** Extensible `User` base class.
  * **Polymorphism:** Overridden virtual methods for role-specific UI rendering.
  * **Encapsulation:** Protected internal wallet states and protected GPS coordinates.

## 🎮 How to Run

1. Clone the repository:
   ```bash
   git clone [https://github.com/yourusername/Tomato-CLI.git](https://github.com/yourusername/Tomato-CLI.git)
