#pragma once
#include <vector>
#include <coroutine>
#include "frontend/debug.h"

namespace js {
    using namespace std;
    template <typename T>
    class Stack {
    private:
        vector<T> m_Data;
    public:
        void Push(T item) {m_Data.push_back(item);};
        T Pop() {
            auto ret = m_Data.back();
            m_Data.pop_back();
            return ret;
        };
        T Peek() {return m_Data[m_Data.size() - 1];}
        bool IsEmpty() {return m_Data.size() == 0;}
        int Size() {return m_Data.size();}
        T& operator[](int index) {return m_Data[index];}
        const T& operator[](int index) const {return m_Data[index];}
        std::vector<T>& asVector() {return m_Data;}
    };

    inline bool isNumeric(const std::string& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
    }
    inline bool SearchAtSpecificIndex(const std::string& str, size_t index, const std::string& ch) {

        VERIFY(index >= 0 && index < str.length(),"index out of range");

        auto sub = str.substr(index, ch.length());

        if (sub.length() < ch.length()) {
            return false;
        }

        return sub == ch;


    }


    inline bool only_consist_of(const std::string& str, const char allowedChars) {
        int MatchCount = 0;
        for (auto ch : str) {
            if (ch == allowedChars) {
                MatchCount++;
            }
        }
        if (MatchCount == str.length())
            return true;
        return false;
    }

    inline int absolute(int value) {
        return value < 0 ? -value : value;
    }

    inline std::vector<std::string> Split(const std::string& str, char delimiter) {
        // Geçici bir dizi oluştur

        std::vector<std::string> result;
        std::vector<int> poses;

        size_t start = 0;

        // Diziyi ayraç karakterine göre bölmek
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == delimiter) {
                result.push_back(str.substr(start, i - start));
                start = i + 1;
            }
        }

        // Son parçayı ekleyin


        result.push_back(str.substr(start, str.length() - start));

        int j = 0;

        for (auto word : result) {
            if (only_consist_of(word,' '))
                poses.push_back(j);
            j++;
        }

        for (int k = 0; k < poses.size(); k++) {
            poses[k] -= k;
        }

        for (auto pos : poses) result.erase(result.begin() + pos);

        return result;
    }

    template <class T>
        int FindIndex (std::vector<T> vec, T item) {
            int index = 0;
            for (auto it : vec) {
                if (it == item)
                    return index;
                index++;
            }
        return -1;
    }


    class packer {
    public:
        static std::array<uint8_t,2> pack16(uint16_t value) {
            std::array<uint8_t,2> bytes;
            for (int i = 0; i < 2; ++i) {
                bytes[i] = (value >> (i * 8)) & 0xFF;
            }
            return bytes;
        }


        static std::array<uint8_t,4> pack32(uint32_t value) {
            std::array<uint8_t,4> bytes;
            for (int i = 0; i < 4; ++i) {
                bytes[i] = (value >> (i * 8)) & 0xFF;
            }
            return bytes;
        }

        static uint8_t* pack64(uint64_t value) {
            uint8_t* bytes;
            for (int i = 0; i < 8; ++i) {
                bytes[i] = (value >> (i * 8)) & 0xFF;
            }
            return bytes;
        }
    };


}
