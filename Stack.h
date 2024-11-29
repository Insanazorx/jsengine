#pragma once
#include <vector>
#include <list>

namespace Util {
    using namespace std;
    template <typename T>
    class Stack {
    private:
        vector<T> m_Data;
    public:
        void Push(T item) {m_Data.push_back(item);};
        T Pop() {m_Data.pop_back(m_Data.getSize() - 1);};
        T Peek() {return m_Data[m_Data.getSize() - 1];};
        bool IsEmpty() {return m_Data.getSize() == 0;};
        int Size() {return m_Data.getSize();};
        T& operator[](int index) {return m_Data[index];};
        const T& operator[](int index) const {return m_Data[index];};
        list<T> ConvertTostd::list() {return m_Data.ConvertTostd::list();}
    };

}
