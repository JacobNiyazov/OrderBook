#pragma once

#include <iostream>
#include <string>
#include <iomanip>

class Print {
public:
    // Generic print function
    template<typename T>
    static void print(const T& message);

    // Print with a newline
    template<typename T>
    static void println(const T& message);

    //// Print a separator line
    //static void printSeparator();

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    static void bookln(const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6);
};

// Implementation of the generic print function
template<typename T>
void Print::print(const T& message) {
    std::cout << message;
}

// Implementation of print with a newline
template<typename T>
void Print::println(const T& message) {
    std::cout << message << std::endl;
}
template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void Print::bookln(const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6) {
    const int width = 15;

    std::cout << std::left << std::setw(width) << v1
        << std::left << std::setw(width) << v2
        << std::left << std::setw(width) << v3
        << " | "
        << std::right << std::setw(width) << v4
        << std::right << std::setw(width) << v5
        << std::right << std::setw(width) << v6 << std::endl;
}
//// Implementation of print separator
//void Print::printSeparator() {
//    std::cout << "--------------------------------" << std::endl;
//}

