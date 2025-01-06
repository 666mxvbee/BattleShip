# Battleship (C++)

**Описание:**  
Этот проект — классическая игра «Морской бой» на C++, где два игрока (Master и Slave) взаимодействуют друг с другом с помощью служебных файлов. 

- **Master** определяет размеры поля, количество кораблей и начинает игру.  
- **Slave** автоматически считывает все настройки от Master.  

## Содержание
1. [Особенности проекта](#особенности-проекта)
2. [Структура проекта](#структура-проекта)
3. [Сборка](#сборка)
4. [Команды Master](#команды-master)
5. [Команды Slave](#команды-slave)
6. [Очередность ходов](#очередность-ходов)
7. [Пример процесса игры](#пример-процесса-игры)
8. [Служебные файлы](#служебные-файлы)

---

## Особенности проекта

- Игра расчитана на **два независимых терминала**, каждый исполняемый файл (`master_main.exe` и `slave_main.exe`) запускается отдельно.
- **Master** задаёт настройки и видит только собственные корабли.  
- **Slave** получает настройки автоматически и также видит только собственные корабли.  
- **Чужие** корабли (противника) и их координаты **не** выводятся, чтобы исключить «читы».  
- Логика ходов организована через файл `queue.txt`:  
  - `1` означает ход Master  
  - `2` означает ход Slave

---

## Структура проекта

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
├── CMakeLists.txt
└── README.md
```

- bin содержит исполняемые точки входа:
  - master_main.cpp
  - slave_main.cpp

- lib хранит логику игры (классы Master, Slave, Settings, RandomShips и т. д.).

- CMakeLists.txt — скрипт для сборки через CMake.

## Сборка

1. Убедитесь, что у вас установлен CMake (3.10+) и компилятор C++ (например, g++ или clang++).

2. В корне репозитория (папка Battleship/) выполните:
   ```bash copycode
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

3. После сборки в папке build/Release появятся файлы:
- __master_main.exe___
- ___slave_main.exe___

## Команды Master

- set mode master
- set width <N>
- set height <N>
- set count <type> <count> (например, set count 4 1 — добавить 1 корабль длины 4)
- start — начинает игру, расставляет корабли Master и ждёт, пока Slave расставит свои.
- shot X Y — совершить выстрел. Если промах, ход перейдёт к Slave; если попадание — ход останется у Master.
- stop — остановить текущую игру.
- exit — полностью выйти из приложения.

### Пример использования:
```bash copycode
Master: Enter commands (e.g. set mode master, set width 10, set height 10, set count 4 1, start):
> set mode master
> set width 10
> set height 10
> set count 1 4   # Четыре корабля длины 1
> set count 2 2   # Два корабля длины 2
> start
```

## Команды Slave
- shot X Y — сделать выстрел, если это ход Slave.
- exit — выйти из игры.
- ___Все прочие настройки (ширина, высота и т. д.) Slave не задаёт — они приходят автоматически от Master.___

## Очередность ходов
- Хранится в файле queue.txt: 1 = Master, 2 = Slave.
- Если игрок промахивается, ход переходит к противнику; если попал или потопил корабль, ход остаётся у него.
- При смене очереди в терминале нового игрока появляется строка вида:
   ```arduino copycode
   [Slave] Now it's your turn.
   ```
   или

   ```arduino copycode
   [Master] Now it's your turn.
   ```

## Пример процесса игры 
1. ___Slave запускается первым и сообщает:___
   ```vbnet copycode
   Slave: Waiting for game to start...
   ```

2. ___Master запускается вторым и задаёт команды:___
   ```shell copycode
   > set mode master
   > set width 10
   > set height 10
   > set count 1 4
   > set count 2 2
   > start
   ```

3. ___Master расставляет собственные корабли (master_ships.txt), затем ждёт, пока Slave расставит свои (slave_ships.txt).___

4. ___Когда расстановка завершена, начинается фаза выстрелов:
Master вводит shot X Y и сразу видит Master: You hit a Slave ship at (X, Y). или Master: You missed....
При промахе ход переходит к Slave, и в терминале Slave появляется:___
   ```arduino copycode
   [Slave] Now it's your turn.
   ```

5. ___Slave делает выстрел командой shot X Y, получает ответ (You hit, You missed, и т. д.).___
6. ___Как только все корабли одного из игроков будут уничтожены, игра выводит победителя (например, Master: All Slave ships have been sunk. Master wins!).___
7. ___При желании можно ввести stop (прекращает игру, но остаётесь в приложении) или exit (полный выход).___

## Служебные файлы
В папке ../game_data/ (относительно исполняемых .exe) будут создаваться:

- **settings.txt** — настройки (ширина, высота, количество кораблей и т. д.).
- **master_ships.txt** / **slave_ships.txt** — позиции кораблей Master/Slave.
- **queue.txt** — хранит текущего игрока (1 или 2).
- **shots.txt** — логи выстрелов.
