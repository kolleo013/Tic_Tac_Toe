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