
#pragma once
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <typeinfo>

#define BRK() raise(SIGTRAP)

#define GREEN_TEXT "\033[1;32m"
#define RESET_TEXT "\033[0m"
#define DEBUG(x) do {std::cout <<"[" GREEN_TEXT "+" RESET_TEXT "] " << x << std::endl;char _; std::cin >> std::noskipws >> _ ;} while (0)
#define ERR(x) do {std::cerr <<"[" GREEN_TEXT "*" RESET_TEXT "] " << x << std::endl;} while (0)
#define FUNCTION_NAME() (__func__)


#define VERIFY(condition, message) \
do { \
if (!(condition)) { \
std::cerr << GREEN_TEXT "VERIFY failed: " RESET_TEXT << message << "\n" \
<< GREEN_TEXT "Condition: " RESET_TEXT << #condition << "\n" \
<< GREEN_TEXT "File: " RESET_TEXT<< __FILE__ << "\n" \
<< GREEN_TEXT "Line: " RESET_TEXT<< __LINE__ << std::endl; \
std::exit(EXIT_FAILURE); \
} \
} while (0)

#define VERIFY_NOT_REACHED()                                         \
do {                                                             \
std::cerr << "VERIFY_NOT_REACHED: Code should not reach "    \
<< __FILE__ << ":" << __LINE__ << "!\n";           \
std::exit(EXIT_FAILURE);                                                \
} while (0)

#define VERIFY_ONCE(condition, message) \
do { \
static bool isOnceVerified = false; \
if (!(condition) && !isOnceVerified) { \
std::cerr << GREEN_TEXT "VERIFY failed: " RESET_TEXT << message << "\n" \
<< GREEN_TEXT "Condition: " RESET_TEXT << #condition << "\n" \
<< GREEN_TEXT "File: " RESET_TEXT<< __FILE__ << "\n" \
<< GREEN_TEXT "Line: " RESET_TEXT<< __LINE__ << std::endl; \
std::exit(EXIT_FAILURE); \
} \
isOnceVerified = true;\
} while (0)

#define LOGGER_BANNER(x) GREEN_TEXT<< #x <<" " << FUNCTION_NAME() << " " << RESET_TEXT

class Debugger {
public:
    // Genel log fonksiyonu
    template <typename T>
    static void Log(const std::string& varName, const T& value) {
        DEBUG(LOGGER_BANNER([[DEBUGGER]]) << varName << " = " << value);
    }

    // Sınıf üyesi değişkenlerini loglayan fonksiyon
    template <typename T>
    static void Inspect(const T& obj) {
        obj.DebugInfo(); // Her sınıf DebugInfo fonksiyonunu sağlamalı
    }
};

// Macro ile getter ve DebugInfo üretimi
#define DEBUGGER_FRIEND_AND_GETTERS(...)                          \
    friend class Debugger;                                        \
    void DebugInfo() const {                                      \
        Debugger::Log("Class Name", typeid(*this).name());        \
        (void)std::initializer_list<int>{                         \
        (Debugger::Log(# __VA_ARGS__, this->__VA_ARGS__), 0)...}; \
    }


