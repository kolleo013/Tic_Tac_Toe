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

const int RAZMER_POLYA = 3;                     // Размер поля 3x3
const char PUSTAYA_KLETKA = ' ';                // Символ пустой клетки
const char IGROK_X = 'X';                       // Символ первого игрока
const char IGROK_O = 'O';                       // Символ второго игрока
const string IMYA_FAYLA = "sohranenie_igry.txt"; // Имя файла

typedef vector<vector<char> > IgrovoePole;

// Функция для очистки экрана
void ochistit_ekran() {
    system("clear");
}

// Функция для цветного текста
void pechatat_cvetom(const string& tekst, int kod_tsveta) {
    cout << "\033[" << kod_tsveta << "m" << tekst << "\033[0m";
}

// Функция для печати заголовка
void pechatat_zagolovok() {
    ochistit_ekran();
    
    cout << "----------------------------------------\n";
    pechatat_cvetom("        КРЕСТИКИ-НОЛИКИ\n", 33);
    cout << "----------------------------------------\n\n";
}

// Функция для печати текста по центру
void pechatat_po_centru(const string& tekst) {
    int shirina = 40;
    int dlinna_teksta = tekst.length();
    int otstup = (shirina - dlinna_teksta) / 2;
    
    if (otstup < 0) otstup = 0;
    
    cout << string(otstup, ' ') << tekst << endl;
}

// Функция "Нажмите Enter для продолжения"
void ozhidat_enter() {
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Функция для удаления лишних пробелов
string udalit_probely(const string& stroka) {
    size_t nachalo = stroka.find_first_not_of(" \t\n\r");
    if (nachalo == string::npos) return "";
    
    size_t konec = stroka.find_last_not_of(" \t\n\r");
    return stroka.substr(nachalo, konec - nachalo + 1);
}

// Функция для получения правильного числа
int poluchit_chislo(const string& podskazka, int minimum, int maximum) {
    int chislo;
    bool pravilno = false;
    
    while (!pravilno) {
        cout << podskazka;
        string vvod;
        getline(cin, vvod);
        vvod = udalit_probely(vvod);
        
        if (vvod.empty()) {
            pechatat_cvetom("Ошибка: нельзя оставлять поле пустым!\n", 31);
            continue;
        }
        
        // Старый способ проверки - без range-based for
        bool eto_chislo = true;
        for (size_t i = 0; i < vvod.length(); i++) {
            char simvol = vvod[i];
            if (!isdigit(simvol) && simvol != '-') {
                eto_chislo = false;
                break;
            }
        }
        
        if (!eto_chislo) {
            pechatat_cvetom("Ошибка: нужно ввести число, а не буквы!\n", 31);
            continue;
        }
        
        // Пробуем преобразовать строку в число
        // Старый способ без stoi
        char* endptr;
        chislo = strtol(vvod.c_str(), &endptr, 10);
        
        // Проверяем, было ли преобразование успешным
        if (*endptr != '\0') {
            pechatat_cvetom("Ошибка: неправильное число!\n", 31);
            continue;
        }
        
        // Проверяем диапазон
        if (chislo < minimum || chislo > maximum) {
            pechatat_cvetom("Ошибка: число должно быть от " + 
                          to_string(minimum) + " до " + 
                          to_string(maximum) + "!\n", 31);
            continue;
        }
        
        pravilno = true;
    }
    
    return chislo;
}

// Вспомогательная функция для приведения строки к нижнему регистру
string sdelat_malenkimi(const string& str) {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        result[i] = tolower(result[i]);
    }
    return result;
}

// Функция для получения правильного выбора (да/нет)
string poluchit_vybor(const string& podskazka) {
    string vvod;
    bool pravilno = false;
    
    while (!pravilno) {
        cout << podskazka;
        getline(cin, vvod);
        vvod = udalit_probely(vvod);
        
        if (vvod.empty()) {
            pechatat_cvetom("Ошибка: нужно что-то ввести!\n", 31);
            continue;
        }
        
        // Приводим к нижнему регистру
        string vvod_malenkie = sdelat_malenkimi(vvod);
        
        // Проверяем варианты
        if (vvod_malenkie == "да" || vvod_malenkie == "yes" || 
            vvod_malenkie == "д" || vvod_malenkie == "y") {
            return "да";
        } else if (vvod_malenkie == "нет" || vvod_malenkie == "no" || 
                   vvod_malenkie == "н" || vvod_malenkie == "n") {
            return "нет";
        } else {
            pechatat_cvetom("Ошибка: пожалуйста, введите 'да' или 'нет'!\n", 31);
        }
    }
    
    return "";
}

// Создание пустого игрового поля
IgrovoePole sozdat_pustoe_pole() {
    return IgrovoePole(RAZMER_POLYA, vector<char>(RAZMER_POLYA, PUSTAYA_KLETKA));
}

// Красивый вывод игрового поля на экран
void pokazat_pole(const IgrovoePole& pole) {
    cout << "\n";
    cout << "    1   2   3\n";
    
    cout << "  +---+---+---+\n";
    
    for (int stroka = 0; stroka < RAZMER_POLYA; stroka++) {
        cout << char('A' + stroka) << " | ";
        
        for (int stolbec = 0; stolbec < RAZMER_POLYA; stolbec++) {
            char kletka = pole[stroka][stolbec];
            
            if (kletka == IGROK_X) {
                pechatat_cvetom(string(1, kletka), 31);
            } else if (kletka == IGROK_O) {
                pechatat_cvetom(string(1, kletka), 34);
            } else {
                cout << kletka;
            }
            
            cout << " | ";
        }
        
        cout << "\n";
        
        if (stroka < RAZMER_POLYA - 1) {
            cout << "  +---+---+---+\n";
        }
    }
    
    cout << "  +---+---+---+\n\n";
}
// Проверка, правильный ли формат хода
bool proverit_format_hoda(const string& vvod, int& nomer_stroki, int& nomer_stolbtsa) {
    if (vvod.length() != 2) return false;
    
    char bukva_stroki = toupper(vvod[0]);
    char tsifra_stolbtsa = vvod[1];
    
    if (bukva_stroki < 'A' || bukva_stroki > 'C') return false;
    if (tsifra_stolbtsa < '1' || tsifra_stolbtsa > '3') return false;
    
    nomer_stroki = bukva_stroki - 'A';
    nomer_stolbtsa = tsifra_stolbtsa - '1';
    
    return true;
}

// Сделать ход на поле
bool sdelat_hod(IgrovoePole& pole, int stroka, int stolbec, char igrok) {
    if (stroka < 0 || stroka >= RAZMER_POLYA || 
        stolbec < 0 || stolbec >= RAZMER_POLYA) {
        return false;
    }
    
    if (pole[stroka][stolbec] != PUSTAYA_KLETKA) {
        return false;
    }
    
    pole[stroka][stolbec] = igrok;
    return true;
}

// Проверка, есть ли победитель
char proverit_pobeditelya(const IgrovoePole& pole) {
    // Проверка строк
    for (int stroka = 0; stroka < RAZMER_POLYA; stroka++) {
        if (pole[stroka][0] != PUSTAYA_KLETKA && 
            pole[stroka][0] == pole[stroka][1] && 
            pole[stroka][1] == pole[stroka][2]) {
            return pole[stroka][0];
        }
    }
    
    // Проверка столбцов
    for (int stolbec = 0; stolbec < RAZMER_POLYA; stolbec++) {
        if (pole[0][stolbec] != PUSTAYA_KLETKA && 
            pole[0][stolbec] == pole[1][stolbec] && 
            pole[1][stolbec] == pole[2][stolbec]) {
            return pole[0][stolbec];
        }
    }
    
    // Проверка диагоналей
    if (pole[0][0] != PUSTAYA_KLETKA && 
        pole[0][0] == pole[1][1] && 
        pole[1][1] == pole[2][2]) {
        return pole[0][0];
    }
    
    if (pole[0][2] != PUSTAYA_KLETKA && 
        pole[0][2] == pole[1][1] && 
        pole[1][1] == pole[2][0]) {
        return pole[0][2];
    }
    
    return PUSTAYA_KLETKA;
}

// Проверка, заполнено ли всё поле
bool proverit_nichyu(const IgrovoePole& pole) {
    for (int i = 0; i < RAZMER_POLYA; i++) {
        for (int j = 0; j < RAZMER_POLYA; j++) {
            if (pole[i][j] == PUSTAYA_KLETKA) {
                return false;
            }
        }
    }
    return true;
}

// Сохранить игру в файл
bool sohranit_igru(const IgrovoePole& pole, char tekushiy_igrok) {
    ofstream fayl(IMYA_FAYLA);
    
    if (!fayl.is_open()) {
        pechatat_cvetom("Ошибка: не могу создать файл для сохранения!\n", 31);
        return false;
    }
    
    fayl << tekushiy_igrok << endl;
    
    for (int i = 0; i < RAZMER_POLYA; i++) {
        for (int j = 0; j < RAZMER_POLYA; j++) {
            fayl << pole[i][j];
        }
        fayl << endl;
    }
    
    fayl.close();
    return true;
}

// Загрузить игру из файла
bool zagruzit_igru(IgrovoePole& pole, char& tekushiy_igrok) {
    ifstream fayl(IMYA_FAYLA);
    
    if (!fayl.is_open()) {
        pechatat_cvetom("Ошибка: файл сохранения не найден!\n", 31);
        return false;
    }
    
    string liniya;
    if (!getline(fayl, liniya) || liniya.empty()) {
        pechatat_cvetom("Ошибка: файл поврежден!\n", 31);
        return false;
    }
    tekushiy_igrok = liniya[0];
    
    int nomer_stroki = 0;
    while (getline(fayl, liniya) && nomer_stroki < RAZMER_POLYA) {
        if (liniya.length() != RAZMER_POLYA) {
            pechatat_cvetom("Ошибка: неправильный формат поля в файле!\n", 31);
            return false;
        }
        
        for (int stolbec = 0; stolbec < RAZMER_POLYA; stolbec++) {
            pole[nomer_stroki][stolbec] = liniya[stolbec];
        }
        nomer_stroki++;
    }
    
    fayl.close();
    
    if (nomer_stroki != RAZMER_POLYA) {
        pechatat_cvetom("Ошибка: неполное поле в файле!\n", 31);
        return false;
    }
    
    return true;
}

// Запуск тестов программы
void zapustit_testy() {
    pechatat_zagolovok();
    pechatat_cvetom("=== ТЕСТИРОВАНИЕ ПРОГРАММЫ ===\n", 33);
    
    int proydeno_testov = 0;
    int vsego_testov = 8;
    
    cout << "\nТест 1: Создание пустого поля... ";
    IgrovoePole test_pole = sozdat_pustoe_pole();
    bool test1_ok = true;
    for (int i = 0; i < RAZMER_POLYA; i++) {
        for (int j = 0; j < RAZMER_POLYA; j++) {
            if (test_pole[i][j] != PUSTAYA_KLETKA) {
                test1_ok = false;
            }
        }
    }
    if (test1_ok) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 2: Проверка хода A1... ";
    int stroka, stolbec;
    bool test2_ok = proverit_format_hoda("A1", stroka, stolbec);
    if (test2_ok && stroka == 0 && stolbec == 0) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 3: Проверка неправильного хода D4... ";
    bool test3_ok = !proverit_format_hoda("D4", stroka, stolbec);
    if (test3_ok) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 4: Выполнение хода... ";
    test_pole[0][0] = PUSTAYA_KLETKA;
    bool test4_ok = sdelat_hod(test_pole, 0, 0, IGROK_X);
    if (test4_ok && test_pole[0][0] == IGROK_X) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 5: Проверка победы... ";
    IgrovoePole pole_pobedy = sozdat_pustoe_pole();
    pole_pobedy[0][0] = IGROK_X;
    pole_pobedy[0][1] = IGROK_X;
    pole_pobedy[0][2] = IGROK_X;
    char pobeditel = proverit_pobeditelya(pole_pobedy);
    if (pobeditel == IGROK_X) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 6: Проверка ничьей... ";
    IgrovoePole pole_nichyi = sozdat_pustoe_pole();
    pole_nichyi[0][0] = IGROK_X; 
    pole_nichyi[0][1] = IGROK_O; 
    pole_nichyi[0][2] = IGROK_X;
    pole_nichyi[1][0] = IGROK_O; 
    pole_nichyi[1][1] = IGROK_X; 
    pole_nichyi[1][2] = IGROK_O;
    pole_nichyi[2][0] = IGROK_O; 
    pole_nichyi[2][1] = IGROK_X; 
    pole_nichyi[2][2] = IGROK_O;
    bool nichya = proverit_nichyu(pole_nichyi) && 
                  proverit_pobeditelya(pole_nichyi) == PUSTAYA_KLETKA;
    if (nichya) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 7: Граничные значения... ";
    bool pravilno1 = proverit_format_hoda("A1", stroka, stolbec);
    bool pravilno2 = proverit_format_hoda("C3", stroka, stolbec);
    bool nepravilno1 = proverit_format_hoda("A0", stroka, stolbec);
    bool nepravilno2 = proverit_format_hoda("D1", stroka, stolbec);
    if (pravilno1 && pravilno2 && !nepravilno1 && !nepravilno2) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "Тест 8: Ввод букв вместо чисел... ";
    bool nepravilno3 = proverit_format_hoda("AB", stroka, stolbec);
    if (!nepravilno3) {
        pechatat_cvetom("ПРОЙДЕН ✓\n", 32);
        proydeno_testov++;
    } else {
        pechatat_cvetom("ПРОВАЛ ✗\n", 31);
    }
    
    cout << "\n----------------------------------------\n";
    cout << "РЕЗУЛЬТАТ: " << proydeno_testov << " из " << vsego_testov << " тестов пройдены\n";
    
    if (proydeno_testov == vsego_testov) {
        pechatat_cvetom("\nВсе тесты успешно пройдены! Программа работает правильно.\n", 32);
    } else {
        pechatat_cvetom("\nЕсть проблемы в программе! Некоторые тесты не прошли.\n", 31);
    }
    
    ozhidat_enter();
}