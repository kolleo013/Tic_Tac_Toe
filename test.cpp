#include <iostream>    // Для ввода и вывода текста
#include <vector>      // Для работы с умными массивами
#include <string>      // Для работы со словами
#include <fstream>     // Для работы с файлами
#include <cctype>      // Для проверки символов
#include <algorithm>   // Для полезных операций
#include <limits>      // Для работы с числами
#include <cstdlib>     // Для системных команд
#include <ctime>       // Для работы со временем

using namespace std;

const int BOARD_SIZE = 3; // Размер поля 3x3
const char EMPTY_CELL = ' '; // Символ пустой клетки
const char PLAYER_X = 'X'; // Символ первого игрока
const char PLAYER_O = 'O'; // Символ второго игрока
const string SAVE_FILE = "saved_game.txt"; // Имя файла

typedef vector<vector<char> > GameBoard;

// Функция для очистки экрана
void clearScreen() {
    system("clear");
}

// Функция для цветного текста
void printColor(const string& text, int colorCode) {
    cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

// Функция для печати заголовка
void printHeader() {
    clearScreen();
    
    cout << "----------------------------------------\n";
    printColor("        КРЕСТИКИ-НОЛИКИ\n", 33);
    cout << "----------------------------------------\n\n";
}

// Функция для печати текста по центру
void printCentered(const string& text) {
    int width = 40;
    int textLength = text.length();
    int padding = (width - textLength) / 2;
    
    if (padding < 0) padding = 0;
    
    cout << string(padding, ' ') << text << endl;
}

// Функция "Нажмите Enter для продолжения"
void waitForEnter() {
    cout << "\nНажмите Enter для продолжения...";
    // Игнорируем все символы в буфере ввода до символа новой строки
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();  // Ждем нажатия Enter
}

// Функция для удаления лишних пробелов
string trimString(const string& str) {
    // Ищем первый символ, который не является пробелом, табуляцией или переводом строки
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";  // Если вся строка из пробелов
    
    // Ищем последний непробельный символ
    size_t end = str.find_last_not_of(" \t\n\r");
    // Вырезаем подстроку от первого до последнего непробельного символа
    return str.substr(start, end - start + 1);
}

// Функция для получения правильного числа
int getValidNumber(const string& prompt, int minVal, int maxVal) {
    int number;
    bool isValid = false;
    
    while (!isValid) {
        cout << prompt;
        string input;
        getline(cin, input);
        input = trimString(input);
        
        if (input.empty()) {
            printColor("Ошибка: нельзя оставлять поле пустым!\n", 31);
            continue;
        }
        
        // Проверяем, что все символы - цифры (или минус для отрицательных чисел)
        bool isNumber = true;
        for (size_t i = 0; i < input.length(); i++) {
            char symbol = input[i];
            if (!isdigit(symbol) && symbol != '-') {
                isNumber = false;
                break;
            }
        }
        
        if (!isNumber) {
            printColor("Ошибка: нужно ввести число, а не буквы!\n", 31);
            continue;
        }
        
        // Преобразуем строку в число 
        char* endPtr;
        number = strtol(input.c_str(), &endPtr, 10);
        
        // Проверяем успешность преобразования: если endPtr указывает не на конец строки,
        // значит были лишние символы
        if (*endPtr != '\0') {
            printColor("Ошибка: неправильное число!\n", 31);
            continue;
        }
        
        // Проверяем, что число входит в заданный диапазон
        if (number < minVal || number > maxVal) {
            printColor("Ошибка: число должно быть от " + 
                          to_string(minVal) + " до " + 
                          to_string(maxVal) + "!\n", 31);
            continue;
        }
        
        isValid = true;
    }
    
    return number;
}

// Вспомогательная функция для приведения строки к нижнему регистру
string toLowerCase(const string& str) {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        result[i] = tolower(result[i]);  // Преобразуем каждый символ в нижний регистр
    }
    return result;
}

// Функция для получения правильного выбора (да/нет)
string getChoice(const string& prompt) {
    string input;
    bool isValid = false;
    
    while (!isValid) {
        cout << prompt;
        getline(cin, input);
        input = trimString(input);
        
        if (input.empty()) {
            printColor("Ошибка: нужно что-то ввести!\n", 31);
            continue;
        }
        
        // Приводим к нижнему регистру для унификации сравнения
        string lowerInput = toLowerCase(input);
        
        // Проверяем различные варианты написания "да" и "нет"
        if (lowerInput == "да" || lowerInput == "yes" || 
            lowerInput == "д" || lowerInput == "y") {
            return "да";
        } else if (lowerInput == "нет" || lowerInput == "no" || 
                   lowerInput == "н" || lowerInput == "n") {
            return "нет";
        } else {
            printColor("Ошибка: пожалуйста, введите 'да' или 'нет'!\n", 31);
        }
    }
    
    return "";
}

// Создание пустого игрового поля
GameBoard createEmptyBoard() {
    // Создаем двумерный вектор: BOARD_SIZE строк, каждая строка - вектор из BOARD_SIZE пробелов
    return GameBoard(BOARD_SIZE, vector<char>(BOARD_SIZE, EMPTY_CELL));
}

// Красивый вывод игрового поля на экран
void displayBoard(const GameBoard& board) {
    cout << "\n";
    cout << "    1   2   3\n";  // Номера столбцов
    
    cout << "  +---+---+---+\n";  // Верхняя граница
    
    for (int row = 0; row < BOARD_SIZE; row++) {
        // Буква строки: A, B, C (соответствует индексам 0, 1, 2)
        cout << char('A' + row) << " | ";
        
        for (int col = 0; col < BOARD_SIZE; col++) {
            char cell = board[row][col];
            
            // Выводим символ с цветом в зависимости от игрока
            if (cell == PLAYER_X) {
                printColor(string(1, cell), 31);  // Красный для X
            } else if (cell == PLAYER_O) {
                printColor(string(1, cell), 34);  // Синий для O
            } else {
                cout << cell;  // Пустая клетка - пробел
            }
            
            cout << " | ";
        }
        
        cout << "\n";
        
        if (row < BOARD_SIZE - 1) {
            cout << "  +---+---+---+\n";  // Разделитель между строками
        }
    }
    
    cout << "  +---+---+---+\n\n";  // Нижняя граница
}

// Проверка, правильный ли формат хода
bool isValidMove(const string& input, int& row, int& col) {
    // Ход должен состоять из 2 символов: буква + цифра
    if (input.length() != 2) return false;
    
    // Первый символ - буква строки (A, B, C)
    char rowChar = toupper(input[0]);
    // Второй символ - цифра столбца (1, 2, 3)
    char colChar = input[1];
    
    // Проверяем допустимость буквы строки
    if (rowChar < 'A' || rowChar > 'C') return false;
    // Проверяем допустимость цифры столбца
    if (colChar < '1' || colChar > '3') return false;
    
    // Преобразуем символы в индексы массива:
    // 'A' -> 0, 'B' -> 1, 'C' -> 2
    // '1' -> 0, '2' -> 1, '3' -> 2
    row = rowChar - 'A';
    col = colChar - '1';
    
    return true;
}

// Сделать ход на поле
bool makeMove(GameBoard& board, int row, int col, char player) {
    // Проверяем, что координаты в пределах поля
    if (row < 0 || row >= BOARD_SIZE || 
        col < 0 || col >= BOARD_SIZE) {
        return false;
    }
    
    // Проверяем, что клетка пуста
    if (board[row][col] != EMPTY_CELL) {
        return false;
    }
    
    // Ставим символ игрока в указанную клетку
    board[row][col] = player;
    return true;
}

// Проверка, есть ли победитель
char checkWinner(const GameBoard& board) {
    // Проверка строк: все три клетки в строке должны быть одинаковы и не пусты
    for (int row = 0; row < BOARD_SIZE; row++) {
        if (board[row][0] != EMPTY_CELL && 
            board[row][0] == board[row][1] && 
            board[row][1] == board[row][2]) {
            return board[row][0];  // Возвращаем символ победителя
        }
    }
    
    // Проверка столбцов: все три клетки в столбце должны быть одинаковы и не пусты
    for (int col = 0; col < BOARD_SIZE; col++) {
        if (board[0][col] != EMPTY_CELL && 
            board[0][col] == board[1][col] && 
            board[1][col] == board[2][col]) {
            return board[0][col];
        }
    }
    
    // Проверка главной диагонали (слева сверху направо вниз)
    if (board[0][0] != EMPTY_CELL && 
        board[0][0] == board[1][1] && 
        board[1][1] == board[2][2]) {
        return board[0][0];
    }
    
    // Проверка побочной диагонали (справа сверху налево вниз)
    if (board[0][2] != EMPTY_CELL && 
        board[0][2] == board[1][1] && 
        board[1][1] == board[2][0]) {
        return board[0][2];
    }
    
    // Если ни одна из проверок не прошла - победителя нет
    return EMPTY_CELL;
}

// Проверка, заполнено ли всё поле (ничья)
bool isDraw(const GameBoard& board) {
    // Проходим по всем клеткам поля
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Если найдена хоть одна пустая клетка - это не ничья
            if (board[i][j] == EMPTY_CELL) {
                return false;
            }
        }
    }
    // Если все клетки заполнены - ничья
    return true;
}

// Сохранить игру в файл
bool saveGame(const GameBoard& board, char currentPlayer) {
    // Открываем файл для записи
    ofstream file(SAVE_FILE);
    
    if (!file.is_open()) {
        printColor("Ошибка: не могу создать файл для сохранения!\n", 31);
        return false;
    }
    
    // Сохраняем текущего игрока (чей ход)
    file << currentPlayer << endl;
    
    // Сохраняем игровое поле: каждая строка поля - отдельная строка в файле
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            file << board[i][j];  // Записываем символ клетки
        }
        file << endl;  // Переход на новую строку после каждой строки поля
    }
    
    file.close();
    return true;
}

// Загрузить игру из файла
bool loadGame(GameBoard& board, char& currentPlayer) {
    // Открываем файл для чтения
    ifstream file(SAVE_FILE);
    
    if (!file.is_open()) {
        printColor("Ошибка: файл сохранения не найден!\n", 31);
        return false;
    }
    
    string line;
    // Читаем первую строку - символ текущего игрока
    if (!getline(file, line) || line.empty()) {
        printColor("Ошибка: файл поврежден!\n", 31);
        return false;
    }
    currentPlayer = line[0];
    
    int rowNum = 0;
    // Читаем следующие 3 строки - игровое поле
    while (getline(file, line) && rowNum < BOARD_SIZE) {
        // Проверяем, что строка имеет правильную длину
        if (line.length() != BOARD_SIZE) {
            printColor("Ошибка: неправильный формат поля в файле!\n", 31);
            return false;
        }
        
        // Копируем символы из строки в игровое поле
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[rowNum][col] = line[col];
        }
        rowNum++;
    }
    
    file.close();
    
    // Проверяем, что загружено полное поле (3 строки)
    if (rowNum != BOARD_SIZE) {
        printColor("Ошибка: неполное поле в файле!\n", 31);
        return false;
    }
    
    return true;
}

// Запуск тестов программы
void runTests() {
    printHeader();
    printColor("----ТЕСТИРОВАНИЕ ПРОГРАММЫ----\n", 33);
    
    int passedTests = 0;
    int totalTests = 8;
    
    cout << "\nТест 1: Создание пустого поля... ";
    GameBoard testBoard = createEmptyBoard();
    bool test1OK = true;
    // Проверяем, что все клетки поля - пустые
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (testBoard[i][j] != EMPTY_CELL) {
                test1OK = false;
            }
        }
    }
    if (test1OK) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 2: Проверка хода A1... ";
    int row, col;
    // Проверяем, что A1 правильно распознается как координата (0, 0)
    bool test2OK = isValidMove("A1", row, col);
    if (test2OK && row == 0 && col == 0) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 3: Проверка неправильного хода D4... ";
    // Проверяем, что некорректный ход правильно отвергается
    bool test3OK = !isValidMove("D4", row, col);
    if (test3OK) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 4: Выполнение хода... ";
    // Очищаем тестовую клетку и пробуем поставить туда X
    testBoard[0][0] = EMPTY_CELL;
    bool test4OK = makeMove(testBoard, 0, 0, PLAYER_X);
    if (test4OK && testBoard[0][0] == PLAYER_X) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 5: Проверка победы... ";
    // Создаем поле с выигрышной комбинацией для X
    GameBoard winBoard = createEmptyBoard();
    winBoard[0][0] = PLAYER_X;
    winBoard[0][1] = PLAYER_X;
    winBoard[0][2] = PLAYER_X;
    char winner = checkWinner(winBoard);
    // Проверяем, что функция правильно определяет победителя
    if (winner == PLAYER_X) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 6: Проверка ничьей... ";
    // Создаем поле без победителя, но полностью заполненное
    GameBoard drawBoard = createEmptyBoard();
    drawBoard[0][0] = PLAYER_X; drawBoard[0][1] = PLAYER_O; drawBoard[0][2] = PLAYER_X;
    drawBoard[1][0] = PLAYER_O; drawBoard[1][1] = PLAYER_X; drawBoard[1][2] = PLAYER_O;
    drawBoard[2][0] = PLAYER_O; drawBoard[2][1] = PLAYER_X; drawBoard[2][2] = PLAYER_O;
    // Проверяем, что поле признается ничьей (заполнено и нет победителя)
    bool isDrawGame = isDraw(drawBoard) && 
                  checkWinner(drawBoard) == EMPTY_CELL;
    if (isDrawGame) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 7: Граничные значения... ";
    // Проверяем минимальные и максимальные допустимые координаты
    bool valid1 = isValidMove("A1", row, col);   // Минимальные координаты
    bool valid2 = isValidMove("C3", row, col);   // Максимальные координаты
    bool invalid1 = isValidMove("A0", row, col); // Недопустимая цифра
    bool invalid2 = isValidMove("D1", row, col); // Недопустимая буква
    if (valid1 && valid2 && !invalid1 && !invalid2) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 8: Ввод букв вместо чисел... ";
    // Проверяем обработку явно некорректного ввода
    bool invalid3 = isValidMove("AB", row, col);
    if (!invalid3) {
        printColor("ПРОЙДЕН ✓\n", 32);
        passedTests++;
    } else {
        printColor("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "\n----------------------------------------\n";
    cout << "РЕЗУЛЬТАТ: " << passedTests << " из " << totalTests << " тестов пройдены\n";
    
    // Выводим итоговое сообщение в зависимости от результатов тестирования
    if (passedTests == totalTests) {
        printColor("\nВсе тесты успешно пройдены! Программа работает правильно.\n", 32);
    } else {
        printColor("\nЕсть проблемы в программе! Некоторые тесты не прошли.\n", 31);
    }
    
    waitForEnter();
}

// Показать правила игры
void showRules() {
    printHeader();
    printColor("----ПРАВИЛА ИГРЫ----\n", 33);
    
    cout << "\n1. Играют два игрока: ";
    printColor("X", 31);
    cout << " и ";
    printColor("O\n", 34);
    
    cout << "2. Игроки ходят по очереди\n";
    cout << "3. Для хода введите координаты клетки:\n";
    cout << "   - Буква строки (A, B, C)\n";
    cout << "   - Цифра столбца (1, 2, 3)\n";
    cout << "   Пример: ";
    printColor("A1", 32);
    cout << ", ";
    printColor("B2", 32);
    cout << ", ";
    printColor("C3\n", 32);
    
    cout << "\n4. Победит тот, кто первым поставит\n";
    cout << "   3 своих символа в ряд:\n";
    cout << "   - по горизонтали\n";
    cout << "   - по вертикали\n";
    cout << "   - по диагонали\n";
    
    cout << "\n5. Если все 9 клеток заполнены,\n";
    cout << "   но нет победителя - ";
    printColor("НИЧЬЯ!\n", 34);
    
    cout << "\n6. Во время игры можно использовать команды:\n";
    cout << "   - ";
    printColor("save", 32);
    cout << " - сохранить игру\n";
    cout << "   - ";
    printColor("menu", 32);
    cout << " - выйти в главное меню\n";
    cout << "   - ";
    printColor("help", 32);
    cout << " - показать правила\n";
    
    waitForEnter();
}

// Основная функция игры
void playGame() {
    GameBoard board = createEmptyBoard();
    char currentPlayer = PLAYER_X;
    bool gameOver = false;
    int moveCount = 0;
    
    // Выбор, кто ходит первым
    printHeader();
    cout << "Кто будет ходить первым?\n";
    cout << "1. Игрок X (крестики)\n";
    cout << "2. Игрок O (нолики)\n";
    cout << "3. Случайный выбор\n";
    
    int choice = getValidNumber("Ваш выбор (1-3): ", 1, 3);
    
    if (choice == 1) {
        currentPlayer = PLAYER_X;
    } else if (choice == 2) {
        currentPlayer = PLAYER_O;
    } else {
        // Случайный выбор первого игрока
        srand(static_cast<unsigned int>(time(NULL)));
        currentPlayer = (rand() % 2 == 0) ? PLAYER_X : PLAYER_O;
        cout << "\nСлучайный выбор: начинает игрок ";
        printColor(string(1, currentPlayer) + "\n", 
                       (currentPlayer == PLAYER_X) ? 31 : 34);
        waitForEnter();
    }
    
    // Основной игровой цикл
    while (!gameOver) {
        printHeader();
        displayBoard(board);
        
        cout << "Ход #" << (moveCount + 1) << "\n";
        cout << "Текущий игрок: ";
        if (currentPlayer == PLAYER_X) {
            printColor("X (крестики)\n", 31);
        } else {
            printColor("O (нолики)\n", 34);
        }
        
        cout << "\nВведите ход (например, A1) или команду: ";
        string input;
        getline(cin, input);
        input = trimString(input);
        
        // Проверка команд
        if (input == "help" || input == "Help") {
            showRules();
            continue;
        }
        
        if (input == "menu" || input == "Menu") {
            cout << "\nВыйти в главное меню? (да/нет): ";
            string answer = getChoice("");
            if (answer == "да") {
                return;
            }
            continue;
        }
        
        if (input == "save" || input == "Save") {
            if (saveGame(board, currentPlayer)) {
                printColor("Игра сохранена в файл: " + SAVE_FILE + "\n", 32);
            }
            waitForEnter();
            continue;
        }
        
        // Проверка правильности хода
        int row, col;
        if (!isValidMove(input, row, col)) {
            printColor("Ошибка: неправильный формат хода!\n", 31);
            cout << "Используйте: A1, B2, C3 и т.д.\n";
            waitForEnter();
            continue;
        }
        
        // Пробуем сделать ход
        if (!makeMove(board, row, col, currentPlayer)) {
            printColor("Ошибка: эта клетка уже занята!\n", 31);
            waitForEnter();
            continue;
        }
        
        moveCount++;
        
        // Проверяем, не закончилась ли игра
        char winner = checkWinner(board);
        if (winner != EMPTY_CELL) {
            printHeader();
            displayBoard(board);
            
            cout << "\n";
            printColor("----------------------------------------\n", 33);
            printColor("     ПОБЕДИЛ ИГРОК " + string(1, winner) + "!\n", 32);
            printColor("----------------------------------------\n", 33);
            gameOver = true;
            
        } else if (isDraw(board)) {
            printHeader();
            displayBoard(board);
            
            cout << "\n";
            printColor("----------------------------------------\n", 33);
            printColor("           НИЧЬЯ!\n", 34);
            printColor("----------------------------------------\n", 33);
            gameOver = true;
            
        } else {
            // Меняем игрока
            currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
        }
    }
    
    cout << "\nВсего ходов: " << moveCount << endl;
    waitForEnter();
}

// Функция для загрузки сохраненной игры
void loadSavedGame() {
    printHeader();
    printColor("----ЗАГРУЗКА ИГРЫ----\n", 33);
    
    GameBoard board = createEmptyBoard();
    char currentPlayer = PLAYER_X;
    
    if (!loadGame(board, currentPlayer)) {
        cout << "\nНе удалось загрузить сохраненную игру.\n";
        waitForEnter();
        return;
    }
    
    cout << "\nИгра успешно загружена!\n";
    cout << "Текущий игрок: " << currentPlayer << "\n";
    waitForEnter();
    
    // Продолжаем игру
    bool gameOver = false;
    int moveCount = 0;
    
    // Подсчитываем ходы, которые уже были сделаны в сохраненной игре
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != EMPTY_CELL) {
                moveCount++;
            }
        }
    }
    
    while (!gameOver) {
        printHeader();
        displayBoard(board);
        
        cout << "Ход #" << (moveCount + 1) << "\n";
        cout << "Текущий игрок: " << currentPlayer << "\n\n";
        
        cout << "Введите ход или команду: ";
        string input;
        getline(cin, input);
        input = trimString(input);
        
        if (input == "menu" || input == "Menu") {
            return;
        }
        
        if (input == "save" || input == "Save") {
            saveGame(board, currentPlayer);
            waitForEnter();
            continue;
        }
        
        int row, col;
        if (!isValidMove(input, row, col)) {
            printColor("Ошибка: неправильный формат!\n", 31);
            waitForEnter();
            continue;
        }
        
        if (!makeMove(board, row, col, currentPlayer)) {
            printColor("Ошибка: клетка уже занята!\n", 31);
            waitForEnter();
            continue;
        }
        
        moveCount++;
        
        char winner = checkWinner(board);
        if (winner != EMPTY_CELL) {
            printHeader();
            displayBoard(board);
            
            cout << "\n";
            printColor("----------------------------------------\n", 33);
            printColor("     ПОБЕДИЛ ИГРОК " + string(1, winner) + "!\n", 32);
            printColor("----------------------------------------\n", 33);
            gameOver = true;
            
        } else if (isDraw(board)) {
            printHeader();
            displayBoard(board);
            
            cout << "\n";
            printColor("----------------------------------------\n", 33);
            printColor("           НИЧЬЯ!\n", 34);
            printColor("----------------------------------------\n", 33);
            gameOver = true;
            
        } else {
            currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
        }
    }
    
    waitForEnter();
}

void mainMenu() {
    srand(static_cast<unsigned int>(time(NULL)));
    
    bool exitProgram = false;
    
    while (!exitProgram) {
        printHeader();
        
        printColor("=== ГЛАВНОЕ МЕНЮ ===\n", 33);
        cout << "\n";
        cout << "1. Новая игра\n";
        cout << "2. Загрузить сохраненную игру\n";
        cout << "3. Правила игры\n";
        cout << "4. Запустить тестирование\n";
        cout << "5. Выйти из программы\n";
        cout << "\n";
        
        int choice = getValidNumber("Выберите пункт меню (1-5): ", 1, 5);
        
        switch (choice) {
            case 1:
                playGame();
                break;
            case 2:
                loadSavedGame();
                break;
            case 3:
                showRules();
                break;
            case 4:
                runTests();
                break;
            case 5:
                exitProgram = true;
                break;
        }
    }
    
    printHeader();
    printColor("Спасибо за игру! До свидания!\n", 32);
    cout << "\n";
}

int main() {
    mainMenu();
    return 0;
}