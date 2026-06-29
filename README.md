2D Console Multiplayer Bomb Game:

A minimalist, high-performance 2D multiplayer game running entirely inside the terminal. Built on top of a classic client-server architecture, this project supports real-time multi-user connections and features dynamic audio event triggers.

---

Features:

* **Real-Time Multiplayer:** Multiple users can connect to the central game server simultaneously, move across the map, and instantly see each other's actions in real time.
* **Dynamic Audio Feedback:** Integrated audio engine triggered by key game events:
  * Play sound when a new player joins the session.
  * Bomb explosion sound effect.
  * Hit registration sound effect when a player gets caught in the blast area.
* **High Optimization (Doubly Linked List):** The active player pool on the server is handled via a doubly linked list structure, allowing for rapid player instantiation/cleanup upon connect or disconnect with minimal memory overhead.
* **HP & Combat Mechanics:** Every player starts with 3 Health Points (HP). Current HP is rendered directly above the player character model in real time. Getting caught inside the bomb blast area reduces the player's health by 1 HP. Once a player's HP reaches 0, they die and disconnect.

---

Requirements & Dependencies:

To successfully compile and run the project, your system must have the following development libraries installed:

1. **ncurses** — Handles terminal manipulation, window context management, and 2D character-based rendering.
2. **OpenAL & ALUT (OpenAL Utility Toolkit)** — Power the underlying audio engine, handling `.wav` asset buffering and real-time audio playback execution.

---

Installation, Configuration & Compilation:

1. Clone the Repository
Clone this repository to all devices participating in the game session:

```
git clone https://github.com/deniska-design/online_2d_game.git
```
```
cd online_2d_game
```

2. Network Configuration
Before compiling, you need to bind the client and server to the correct network locations. Find out the local IP address of the machine running the server, then update the source files:  

In cliennt.cpp
```
const char *ServerIp = "YOUR_SERVER_IP";
```

In server.cpp:  
```
const char *ip = "YOUR_SERVER_IP";
```

3. Running the Game
First, spin up the server component on the host machine:
```
./server
```

Next, launch the client binary on each player's machine:
```
./client
```

---

Execution:

https://github.com/user-attachments/assets/ad4e8f01-a018-4159-b4e2-0784c25dd672

---

License:
This project is open-source. Anyone is completely free to download, modify, use, and distribute this software for personal or educational purposes.
