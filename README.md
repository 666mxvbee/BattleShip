# Battleship (C++)

**Description:**  
This project is a classic Battleship game written in C++, where two players, Master and Slave, interact with each other through auxiliary files.

- **Master** defines the field size, the number of ships, and starts the game.

- **Slave** automatically reads all settings from Master.

## Contents

1. [Project Features](#project-features)
2. [Project Structure](#project-structure)
3. [Build](#build)
4. [Master Commands](#master-commands)
5. [Slave Commands](#slave-commands)
6. [Turn Order](#turn-order)
7. [Example Game Flow](#example-game-flow)
8. [Auxiliary Files](#auxiliary-files)

---

## Project Features

- The game is designed to run in **two independent terminals**. Each executable file (`master_main.exe` and `slave_main.exe`) is launched separately.
- **Master** sets the game configuration and sees only their own ships.
- **Slave** receives the configuration automatically and also sees only their own ships.
- The opponent’s ships and their coordinates are **not** displayed in order to prevent cheating.
- Turn logic is organized through the `queue.txt` file:
  - `1` means it is Master’s turn
  - `2` means it is Slave’s turn

---

## Project Structure

```bash
Battleship/
├── bin/
│   ├── master_main.cpp
│   └── slave_main.cpp
├── lib/
│   ├── Master/
│   ├── Slave/
│   ├── Settings/
│   ├── ReadWriteQueue/
│   ├── Ship/
│   └── RandomShips/
│   ├── Utils/
│   ├── Commands/
├── CMakeLists.txt
└── README.md
```

- `bin` contains the executable entry points:
  - `master_main.cpp`
  - `slave_main.cpp`

- `lib` contains the game logic, including the `Master`, `Slave`, `Settings`, `RandomShips`, and other classes.

- `CMakeLists.txt` is the build script used by CMake.

## Build

1. Make sure that CMake version 3.10+ and a C++ compiler, such as `g++` or `clang++`, are installed.

2. From the repository root directory, `Battleship/`, run:

   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

3. After the build is complete, the following files will appear in the `build/Release` directory:

- `master_main.exe`
- `slave_main.exe`

## Master Commands

- `set mode master`
- `set width <N>`
- `set height <N>`
- `set count <type> <count>`  
  For example, `set count 4 1` adds one ship of length 4.
- `start` — starts the game, places Master’s ships, and waits until Slave places their ships.
- `shot X Y` — makes a shot. If the shot misses, the turn passes to Slave. If the shot hits, Master keeps the turn.
- `stop` — stops the current game.
- `exit` — exits the application completely.

### Usage Example

```bash
Master: Enter commands (e.g. set mode master, set width 10, set height 10, set count 4 1, start):
> set mode master
> set width 10
> set height 10
> set count 1 4   # Four ships of length 1
> set count 2 2   # Two ships of length 2
> start
```

## Slave Commands

- `shot X Y` — makes a shot if it is Slave’s turn.
- `exit` — exits the game.
- **All other settings, such as width, height, and so on, are not set by Slave. They are received automatically from Master.**

## Turn Order

- The turn order is stored in the `queue.txt` file: `1` = Master, `2` = Slave.
- If a player misses, the turn passes to the opponent. If a player hits or sinks a ship, they keep the turn.
- When the turn changes, the new player’s terminal displays a message like:

   ```arduino
   [Slave] Now it's your turn.
   ```

   or

   ```arduino
   [Master] Now it's your turn.
   ```

## Example Game Flow

1. **Slave is launched first and displays:**

   ```vbnet
   Slave: Waiting for game to start...
   ```

2. **Master is launched second and enters the commands:**

   ```shell
   > set mode master
   > set width 10
   > set height 10
   > set count 1 4
   > set count 2 2
   > start
   ```

3. **Master places their own ships in `master_ships.txt`, then waits until Slave places their ships in `slave_ships.txt`.**

4. **When ship placement is complete, the shooting phase begins.**

   Master enters `shot X Y` and immediately sees a message such as:

   ```text
   Master: You hit a Slave ship at (X, Y).
   ```

   or:

   ```text
   Master: You missed.
   ```

   If Master misses, the turn passes to Slave, and the Slave terminal displays:

   ```arduino
   [Slave] Now it's your turn.
   ```

5. **Slave makes a shot using the `shot X Y` command and receives a response, such as `You hit`, `You missed`, and so on.**

6. **Once all ships of one player have been destroyed, the game displays the winner.**

   Example:

   ```text
   Master: All Slave ships have been sunk. Master wins!
   ```

7. **If needed, the player can enter `stop` to stop the current game while staying in the application, or `exit` to exit completely.**

## Auxiliary Files

The following files will be created in the `../game_data/` directory relative to the executable `.exe` files:

- **settings.txt** — stores settings such as width, height, number of ships, and so on.
- **master_ships.txt** / **slave_ships.txt** — store the positions of Master’s and Slave’s ships.
- **queue.txt** — stores the current player, either `1` or `2`.
- **shots.txt** — stores shot logs.
