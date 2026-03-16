# Word Search Game — C++ OOP Semester Project

**FAST NUCES Faisalabad-Chiniot Campus**  
**Course: Object Oriented Programming**

---

## About the Project

A console-based Word Search Game built in C++ where players find hidden words in a 2D grid. Words are placed horizontally, vertically, and diagonally. The game features two difficulty modes, three progressive levels, a lives system, and a persistent high score tracker.

---

## How to Play

1. Select **Start Game** from the main menu
2. Enter your **Name** and **Roll Number**
3. Choose difficulty: **Easy** or **Hard**
4. A grid of letters will appear on screen
5. Find hidden words in the grid (horizontal, vertical, diagonal)
6. Type the word and press Enter
7. Complete all 3 levels before running out of lives!

---

## Game Modes

| Mode | Word Length | Levels | Grid Sizes |
|------|------------|--------|------------|
| Easy | 2–4 letters | 3 | 10x10, 15x15, 20x20 |
| Hard | 5–7 letters | 5 | 10x10, 15x15, 20x20 |

### Level Requirements
| Level | Words to Find |
|-------|--------------|
| 1     | 3 words      |
| 2     | 5 words      |
| 3     | 7 words      |

---

## OOP Concepts Used

- ✅ Classes & Objects
- ✅ Inheritance (`BaseGrid` → `Grid`)
- ✅ Polymorphism & Virtual Functions
- ✅ Operator Overloading (`+=`, `<<`)
- ✅ Friend Functions
- ✅ Constructors & Destructors
- ✅ Dynamic Memory Allocation
- ✅ Exception Handling
- ✅ File I/O (word lists & high scores)
- ✅ 3-File Structure (`.h`, `imp.cpp`, `main.cpp`)

---

## Project Structure

```
WordGame/
├── Game.h                  # Class declarations & struct definitions
├── imp.cpp                 # Class implementations
├── word_search_game.cpp    # Main entry point
├── easy_words.txt          # 50 words (2–4 letters)
├── hard_words.txt          # 50 words (5–7 letters)
└── highscore.txt           # Auto-generated high score file
```

---

## How to Run

### Dev-C++ (Recommended)
1. Clone or download this repository
2. Open the project in **Dev-C++**
3. Add all files to the project
4. **Important:** Copy `easy_words.txt`, `hard_words.txt`, and `highscore.txt` into the `Debug/` folder
5. Press **F11** to Compile & Run

### Manual Compilation (MinGW)
```bash
g++ word_search_game.cpp imp.cpp -o WordGame.exe
```

---

## Screenshots



---

## Developers

| Name | Roll Number |
|------|------------|
| Aizazullah | 23F-0664 |
| Um-e-Habiba | 23F-0740 |

---
