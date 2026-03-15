#include "Game.h"

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// ===================== ScoreTracker =====================

ScoreTracker::ScoreTracker() : score(0) {}

void ScoreTracker::updateScore(int points) {
    score += points;
}

int ScoreTracker::getScore() const {
    return score;
}

void ScoreTracker::resetScore() {
    score = 0;
}

ostream& operator<<(ostream& os, const ScoreTracker& tracker) {
    os << "Score: " << tracker.score;
    return os;
}

ScoreTracker& ScoreTracker::operator+=(int points) {
    score += points;
    return *this;
}

// ===================== WordValidator =====================

bool WordValidator::isValidWord(string word, string wordList[], int wordCount) {
    for (int i = 0; i < wordCount; i++) {
        if (wordList[i] == word) {
            return true;
        }
    }
    return false;
}

// ===================== BaseGrid =====================

BaseGrid::BaseGrid(int gridSize) : size(gridSize) {}

// ===================== Grid =====================

Grid::Grid(int gridSize) : BaseGrid(gridSize) {
    grid = new char*[size];
    for (int i = 0; i < size; i++) {
        grid[i] = new char[size];
        for (int j = 0; j < size; j++) {
            grid[i][j] = '.';
        }
    }
}

Grid::Grid(const Grid& other) : BaseGrid(other.size) {
    grid = new char*[size];
    for (int i = 0; i < size; i++) {
        grid[i] = new char[size];
        for (int j = 0; j < size; j++) {
            grid[i][j] = other.grid[i][j];
        }
    }
}

Grid::~Grid() {
    for (int i = 0; i < size; i++) {
        delete[] grid[i];
    }
    delete[] grid;
}

bool Grid::tryPlaceWord(const string& word, int row, int col, int dRow, int dCol) {
    int len = (int)word.length();

    // Check bounds
    int endRow = row + dRow * (len - 1);
    int endCol = col + dCol * (len - 1);
    if (endRow < 0 || endRow >= size || endCol < 0 || endCol >= size) {
        return false;
    }

    // Check no conflict with already placed letters (allow overlap only if same letter)
    for (int i = 0; i < len; i++) {
        char existing = grid[row + dRow * i][col + dCol * i];
        if (existing != '.' && existing != word[i]) {
            return false;
        }
    }

    // Place the word
    for (int i = 0; i < len; i++) {
        grid[row + dRow * i][col + dCol * i] = word[i];
    }
    return true;
}

void Grid::populateGrid(string words[], int wordCount) {
    srand((unsigned int)time(0));

    // 8 directions: right, down, diagonal down-right, diagonal down-left,
    //               left, up, diagonal up-left, diagonal up-right
    Direction directions[8] = {
        {0,  1}, {1,  0}, {1,  1}, {1, -1},
        {0, -1}, {-1, 0}, {-1, -1}, {-1, 1}
    };

    for (int w = 0; w < wordCount; w++) {
        string word = words[w];
        bool placed = false;
        int maxAttempts = 200;
        int attempts = 0;

        while (!placed && attempts < maxAttempts) {
            int row = rand() % size;
            int col = rand() % size;
            int dirIndex = rand() % 8;

            Direction d = directions[dirIndex];
            placed = tryPlaceWord(word, row, col, d.dRow, d.dCol);
            attempts++;
        }

        if (!placed) {
            // Force horizontal placement at a safe position as fallback
            int len = (int)word.length();
            if (len <= size) {
                int row = rand() % size;
                int col = rand() % (size - len + 1);
                tryPlaceWord(word, row, col, 0, 1);
            }
        }
    }

    // Fill empty cells with random letters
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == '.') {
                grid[i][j] = 'a' + rand() % 26;
            }
        }
    }
}

void Grid::displayGrid() {
    SetColor(FOREGROUND_CYAN);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == '*') {
                SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                cout << grid[i][j] << " ";
                SetColor(FOREGROUND_CYAN);
            } else {
                cout << grid[i][j] << " ";
            }
        }
        cout << endl;
    }
}

bool Grid::findAndMarkWord(const string& word) {
    int len = (int)word.length();

    Direction directions[8] = {
        {0,  1}, {1,  0}, {1,  1}, {1, -1},
        {0, -1}, {-1, 0}, {-1, -1}, {-1, 1}
    };

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            for (int d = 0; d < 8; d++) {
                int dRow = directions[d].dRow;
                int dCol = directions[d].dCol;

                // Check bounds
                int endRow = row + dRow * (len - 1);
                int endCol = col + dCol * (len - 1);
                if (endRow < 0 || endRow >= size || endCol < 0 || endCol >= size) {
                    continue;
                }

                // Match word
                bool match = true;
                for (int i = 0; i < len; i++) {
                    if (grid[row + dRow * i][col + dCol * i] != word[i]) {
                        match = false;
                        break;
                    }
                }

                if (match) {
                    // Replace with stars
                    for (int i = 0; i < len; i++) {
                        grid[row + dRow * i][col + dCol * i] = '*';
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

int Grid::getSize() const {
    return size;
}

// ===================== Game =====================

Game::Game() : lives(5), grid(nullptr), wordCount(0) {
    for (int i = 0; i < 50; i++) wordFound[i] = false;
}

Game::~Game() {
    delete grid;
}

void Game::initializeGame(const string& mode, int level) {
    delete grid;
    grid = nullptr;

    int gridSize;
    if (level == 1) gridSize = 10;
    else if (level == 2) gridSize = 15;
    else gridSize = 20;

    grid = new Grid(gridSize);

    string fileName;
    if (mode == "Easy" || mode == "easy" || mode == "EASY" || mode == "1") {
        fileName = "easy_words.txt";
    } else if (mode == "Hard" || mode == "hard" || mode == "HARD" || mode == "2") {
        fileName = "hard_words.txt";
    } else {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "Invalid mode. Defaulting to Easy." << endl;
        fileName = "easy_words.txt";
    }

    loadWords(fileName);

    for (int i = 0; i < wordCount; i++) wordFound[i] = false;

    grid->populateGrid(wordList, wordCount);
}

void Game::loadWords(const string& fileName) {
    wordCount = 0;
    ifstream file(fileName);
    if (!file.is_open()) {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "ERROR: Cannot open \"" << fileName << "\"" << endl;
        cout << "Make sure the .txt files are in the SAME folder as the .exe!" << endl;
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        system("pause");
        return;
    }
    while (file >> wordList[wordCount] && wordCount < 50) {
        wordCount++;
    }
    file.close();
}

void Game::saveHighScore() {
    HighScore highScores[10];
    int count = 0;

    ifstream inFile("highscore.txt");
    while (count < 10 && inFile >> highScores[count].name >> highScores[count].rollNo >> highScores[count].score) {
        count++;
    }
    inFile.close();

    int currentScore = scoreTracker.getScore();
    bool alreadyTop = (count >= 10 && currentScore <= highScores[count - 1].score);

    if (!alreadyTop) {
        if (count < 10) {
            highScores[count] = { currentUser.name, currentUser.rollNo, currentScore };
            count++;
        } else {
            highScores[9] = { currentUser.name, currentUser.rollNo, currentScore };
        }

        // Bubble sort descending
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (highScores[j].score < highScores[j + 1].score) {
                    HighScore temp = highScores[j];
                    highScores[j] = highScores[j + 1];
                    highScores[j + 1] = temp;
                }
            }
        }

        ofstream outFile("highscore.txt");
        for (int i = 0; i < count; i++) {
            outFile << highScores[i].name << " " << highScores[i].rollNo << " " << highScores[i].score << endl;
        }
        outFile.close();
    }
}



bool Game::playLevel(int level, const string& mode) {
    initializeGame(mode, level);

    if (wordCount == 0) {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "No words loaded. Cannot start level." << endl;
        system("pause");
        return false;
    }

    int wordsToFind = (level == 1) ? 3 : (level == 2) ? 5 : 7;

    // Clamp to available words
    if (wordsToFind > wordCount) wordsToFind = wordCount;

    SetColor(FOREGROUND_YELLOW);
    cout << "\n========================================" << endl;
    cout << "   LEVEL " << level << " - Find " << wordsToFind << " words!" << endl;
    cout << "========================================" << endl;
    system("pause");

    int foundCount = 0;

    while (foundCount < wordsToFind && lives > 0) {
        system("cls");

        SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        cout << "Player: " << currentUser.name << " | Roll No: " << currentUser.rollNo << endl;
        cout << "Level: " << level << " | ";
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "Lives: " << lives << " | ";
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << scoreTracker << " | Words Found: " << foundCount << "/" << wordsToFind << endl;
        cout << endl;

        grid->displayGrid();

        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << "\nEnter a word (search in the grid): ";
        string inputWord;
        cin >> inputWord;

        // Check if word is in the list and not already found
        bool inList = false;
        int wordIdx = -1;
        for (int i = 0; i < wordCount; i++) {
            if (wordList[i] == inputWord && !wordFound[i]) {
                inList = true;
                wordIdx = i;
                break;
            }
        }

        if (!inList) {
            SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            if (validator.isValidWord(inputWord, wordList, wordCount)) {
                cout << "Word already found or not in current level list! Lives -1." << endl;
            } else {
                cout << "Word not in the list! Lives -1." << endl;
            }
            lives--;
            system("pause");
            continue;
        }

        // Search for it in the grid
        bool found = grid->findAndMarkWord(inputWord);

        if (found) {
            wordFound[wordIdx] = true;
            SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            cout << "Great! \"" << inputWord << "\" found and marked!" << endl;
            scoreTracker += 10 * level;
            foundCount++;
        } else {
            SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            cout << "Word \"" << inputWord << "\" is valid but not found in the grid. Lives -1." << endl;
            lives--;
        }

        system("pause");
    }

    system("cls");

    if (foundCount >= wordsToFind) {
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << "========================================" << endl;
        cout << "   Level " << level << " Completed! Well done!" << endl;
        cout << "   " << scoreTracker << endl;
        cout << "========================================" << endl;
        system("pause");
        return true;
    } else {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "========================================" << endl;
        cout << "   Game Over! You ran out of lives." << endl;
        cout << "   " << scoreTracker << endl;
        cout << "========================================" << endl;
        system("pause");
        return false;
    }
}

void Game::displayMenu() {
    SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    cout << "__________________________________________________" << endl;
    cout << "|          WORD SEARCH GAME - MAIN MENU          |" << endl;
    cout << "__________________________________________________" << endl;
    SetColor(FOREGROUND_YELLOW);
    cout << "  1. Start Game" << endl;
    cout << "  2. Instructions" << endl;
    cout << "  3. Highest Scores" << endl;
    cout << "  4. About Us" << endl;
    cout << "  5. Exit" << endl;
    cout << "__________________________________________________" << endl;
}

void Game::start() {
    int choice;
    do {
        system("cls");
        displayMenu();
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            system("cls");
            SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            cout << "Enter your name: ";
            cin.ignore();
            getline(cin, currentUser.name);
            cout << "Enter your roll number: ";
            getline(cin, currentUser.rollNo);

            scoreTracker.resetScore();
            lives = 5;

            SetColor(FOREGROUND_YELLOW);
            cout << "\nChoose Difficulty:" << endl;
            cout << "  1. Easy" << endl;
            cout << "  2. Hard" << endl;
            SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            cout << "Enter choice: ";
            string mode;
            cin >> mode;

            int level = 1;
            bool gameOver = false;

            while (level <= 3 && !gameOver) {
                if (!playLevel(level, mode)) {
                    gameOver = true;
                } else {
                    level++;
                    // Restore 2 lives as bonus on level completion (max 5)
                    if (!gameOver && level <= 3) {
                        lives += 2;
                        if (lives > 5) lives = 5;
                    }
                }
            }

            if (!gameOver && level > 3) {
                system("cls");
                SetColor(FOREGROUND_YELLOW);
                cout << "========================================" << endl;
                cout << " CONGRATULATIONS! All Levels Completed!" << endl;
                cout << " Final " << scoreTracker << endl;
                cout << "========================================" << endl;
            }

            saveHighScore();
            system("pause");

        } else if (choice == 2) {
            displayInstructions();
        } else if (choice == 3) {
            displayHighestScores();
        } else if (choice == 4) {
            aboutUs();
        } else if (choice == 5) {
            SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            cout << "Thanks for playing! Goodbye!" << endl;
            system("pause");
        } else {
            SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            cout << "Invalid choice. Please try again." << endl;
            system("pause");
        }

    } while (choice != 5);
}

void Game::displayInstructions() {
    system("cls");
    SetColor(FOREGROUND_YELLOW);
    cout << "========================================" << endl;
    cout << "           HOW TO PLAY" << endl;
    cout << "========================================" << endl;
    SetColor(FOREGROUND_CYAN);
    cout << "1. A grid of letters is displayed." << endl;
    cout << "2. Hidden words are placed horizontally," << endl;
    cout << "   vertically, or diagonally (all 8 dirs)." << endl;
    cout << "3. A list of words to find is shown." << endl;
    cout << "4. Type the exact word and press Enter." << endl;
    cout << "5. Found words are marked with '*'." << endl;
    cout << "6. You have 5 lives. Wrong answers cost 1." << endl;
    cout << "7. 3 Levels: Easy->Medium->Hard grid sizes." << endl;
    cout << "8. Score = 10 x Level per correct word." << endl;
    cout << "========================================" << endl;
    system("pause");
}

void Game::displayHighestScores() {
    system("cls");
    SetColor(FOREGROUND_YELLOW);
    cout << "========================================" << endl;
    cout << "            HIGH SCORES" << endl;
    cout << "========================================" << endl;

    ifstream inFile("highscore.txt");
    HighScore hs;
    int rank = 1;
    bool hasScores = false;

    while (inFile >> hs.name >> hs.rollNo >> hs.score) {
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << rank << ". " << hs.name << " | " << hs.rollNo << " | Score: " << hs.score << endl;
        rank++;
        hasScores = true;
    }
    inFile.close();

    if (!hasScores) {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "No scores yet. Play a game first!" << endl;
    }

    cout << "========================================" << endl;
    system("pause");
}

void Game::aboutUs() {
    system("cls");
    SetColor(FOREGROUND_YELLOW);
    cout << "========================================" << endl;
    cout << "             ABOUT US" << endl;
    cout << "========================================" << endl;
    SetColor(FOREGROUND_CYAN);
    cout << "Word Search Game" << endl;
    cout << "Developed as a C++ OOP Project." << endl;
    cout << "Features:" << endl;
    cout << "  - 3 Difficulty Levels" << endl;
    cout << "  - Easy & Hard word lists" << endl;
    cout << "  - 8-directional word placement" << endl;
    cout << "  - Persistent High Score system" << endl;
    cout << "  - Score tracking & Lives system" << endl;
    cout << "========================================" << endl;
    system("pause");
}
