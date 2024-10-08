#pragma once
#include <cstdint>
#include "debug.h"

namespace Std {

typedef int size_t;
class String;

template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> { typedef T type; };

template<typename Base, typename Derived>
class is_base_of {
    static char test(Base*);
    static int test(...);

public:
    static const bool value = sizeof(test(static_cast<Derived*>(nullptr))) == sizeof(char);
};

template<typename T> class Vector;
template<typename T> class List;

template<typename T, typename U>
bool is_same_v = false;

template<typename T>
bool is_same_v<T, T> = true;


template <class T> struct remove_reference      {typedef T type;};
template <class T> struct remove_reference<T&>  {typedef T type;};
template <class T> struct remove_reference<T&&> {typedef T type;};
	
template <typename T>
typename remove_reference<T>::type&& Move(T&& arg) {
	return static_cast<typename remove_reference<T>::type&&>(arg);
}
class Function {
private:
    void* m_function;
    void* m_object;

public:
    template<typename ReturnType, typename... Args>
    Function(ReturnType (*func)(Args...)) : m_function(reinterpret_cast<void*>(func)), m_object(nullptr) {}

    template<typename ReturnType, typename ClassType, typename... Args>
    Function(ReturnType (ClassType::*func)(Args...), ClassType* obj) : m_function(reinterpret_cast<void*>(func)), m_object(obj) {}

    template<typename ReturnType, typename... Args>
    ReturnType operator()(Args... args) const {
        if (m_object) {
            return (reinterpret_cast<ReturnType (*)()>(m_function))(args...);
        } else {
            return (reinterpret_cast<ReturnType (*)(Args...)>(m_function))(args...);
        }
    }

    bool isValid() const {
        return m_function != nullptr;
    }

};


template<typename... Types>
class Variant;

template<typename T, typename... Types>
class Variant<T, Types...> {
private:
    union {
        T head;
        Variant<Types...> tail;
    };
    bool isHead;

public:
    Variant() : isHead(false) {}

    Variant(const T& value) : head(value), isHead(true) {}

    template<typename U>
    Variant(const U& value) : tail(value), isHead(false) {}

    ~Variant() {
        if (isHead) {
            head.~T();
        } else {
            tail.~Variant<Types...>();
        }
    }

    template<typename U>
    bool is() const {
        if (is_same_v<T, U>) {
            return isHead;
        } else {
            return !isHead && tail.template is<U>();
        }
    }

    template<typename U>
    U& get() {
        if (is_same_v<T, U>) {
            if (!isHead) throw std::bad_variant_access();
            return head;
        } else {
            if (isHead) throw std::bad_variant_access();
            return tail.template get<U>();
        }
    }

    template<typename U>
    const U& get() const {
        if (is_same_v<T, U>) {
            if (!isHead) throw std::bad_variant_access();
            return head;
        } else {
            if (isHead) throw std::bad_variant_access();
            return tail.template get<U>();
        }
    }
};

template<>
class Variant<> {
public:
    template<typename U>
    bool is() const {
        return false;
    }

    template<typename U>
    U& get() {
        throw std::bad_variant_access(); // TODO: Remove this   
    }

    template<typename U>
    const U& get() const {
            throw std::bad_variant_access(); // TODO: Remove this   
    }
};

inline void Strcpy(char* dest, const char* src) {

    if (dest == nullptr || src == nullptr) {
        return;
    }

    while (*src != '\0') {
        *dest = *src;
        ++dest;
        ++src;
    }

    *dest = '\0';
}

inline size_t Min (size_t len1, size_t len2) {
    return (len1 < len2) ? len1 : len2;
}

inline size_t Strlen(const char* str) {
    if (str == nullptr) {
        return 0; 
    }

    size_t length = 0;
    while (*str != '\0') {
        ++length;
        ++str;
    }
    return length;
}

inline char* Strcat(char* dest, const char* src) {

    if (dest == nullptr || src == nullptr) {
        return dest;
    }

    char* originalDest = dest;

    while (*dest != '\0') {
        ++dest;
    }

    while (*src != '\0') {
        *dest = *src;
        ++dest;
        ++src;
    }
   
    *dest = '\0';

    return originalDest; 
}



inline int Strcmp(const char* str1, const char* str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

inline void* Memmove(void* dest, const void* src, size_t n) {
    unsigned char* dest_ptr = static_cast<unsigned char*>(dest);
    const unsigned char* src_ptr = static_cast<const unsigned char*>(src);

    if (dest_ptr == src_ptr) {
        return dest;
    }

    if (dest_ptr < src_ptr) {
        // Kaynak ve hedef blokların adresleri arasındaki örtüşmeyi kontrol et
        if (src_ptr < dest_ptr + n) {
            // Kaynak ve hedef blokların örtüştüğü durumda ters sırayla kopyalama
            for (size_t i = n; i > 0; --i) {
                dest_ptr[i - 1] = src_ptr[i - 1];
            }
        } else {
            // Örtüşme yoksa doğrudan kopyalama
            for (size_t i = 0; i < n; ++i) {
                dest_ptr[i] = src_ptr[i];
            }
        }
    } else {
        // Kaynak ve hedef blokların adresleri arasındaki örtüşmeyi kontrol et
        if (src_ptr + n > dest_ptr) {
            // Kaynak ve hedef blokların örtüştüğü durumda ters sırayla kopyalama
            for (size_t i = n; i > 0; --i) {
                dest_ptr[i - 1] = src_ptr[i - 1];
            }
        } else {
            // Örtüşme yoksa doğrudan kopyalama
            for (size_t i = 0; i < n; ++i) {
                dest_ptr[i] = src_ptr[i];
            }
        }
    }

    return dest;
}

inline void* Memcpy(void* dest, const void* src, size_t n) {
    unsigned char* dest_ptr = static_cast<unsigned char*>(dest);
    const unsigned char* src_ptr = static_cast<const unsigned char*>(src);

    for (size_t i = 0; i < n; ++i) {
        dest_ptr[i] = src_ptr[i];
    }

    return dest;
}

inline const char* Strpbrk(const char* str1, const char* str2) {
    // İlk döngü, str1 içindeki her karakter için
    while (*str1) {
        const char* temp = str2; // str2'yi baştan başlat

        // İkinci döngü, str2 içindeki her karakter için
        while (*temp) {
            if (*str1 == *temp) {
                return str1; // Eşleşme bulunduğunda str1'in adresini döndür
            }
            ++temp;
        }

        // str2 içinde eşleşme bulunamadıysa str1'i bir sonraki karaktere ilerlet
        ++str1;
    }

    return nullptr; // Eşleşme bulunamadı
}

inline int Strncmp(const char* str1, const char* str2, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (str1[i] != str2[i]) {
            return (str1[i] < str2[i]) ? -1 : 1;
        }
        if (str1[i] == '\0' || str2[i] == '\0') {
            break;
        }
    }
    return 0;
}

inline const char* Strstr(const char* str, const char* substr) {
    if (*substr == '\0') // Eğer alt dize boşsa, ana dizeyi döndür
        return str;

    while (*str) {
        const char* p = str;
        const char* q = substr;

        while (*p && *q && *p == *q) {
            p++;
            q++;
        }

        if (*q == '\0') // Alt dize tamamen eşleşti, bu durumda ana dizinin başlangıç konumunu döndür
            return str;

        str++;
    }

    return nullptr; // Alt dize bulunamadı
}

inline bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}

inline int Atoi(const char* str) {
    int result = 0;
    int sign = 1; // İşaret bilgisini saklamak için kullanılır

    // Boşlukları atla
    while (*str == ' ') {
        ++str;
    }

    // İşaret belirleme (+ veya -)
    if (*str == '-') {
        sign = -1;
        ++str;
    } else if (*str == '+') {
        ++str;
    }

    // Karakterleri okuyarak sayıyı oluşturma
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        ++str;
    }

    // İşaret uygulama
    return sign * result;
}

template<typename T>
T&& Forward(typename remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}





template <typename T>
class Vector {
private:
    T* data;
    size_t size;
    size_t capacity;

public:
    Vector() : data(nullptr), size(0), capacity(0) {}

    ~Vector() {
        delete[] data;
    }

    void push_back(const T& element) {
        if (size >= capacity) {
            capacity = (capacity == 0) ? 1 : capacity * 2;
            T* newData = new T[capacity];
            

            for (size_t i = 0; i < size; ++i) {
                newData[i] = data[i];
            }

            delete[] data;
            data = newData;
        }

        data[size++] = element;
    }

    Vector<T>& operator=(const Vector<T>& other) {
        if (this != &other) { // self-assignment kontrolü
            // mevcut belleği temizle
            delete[] data;
            
            // diğer vektörden boyut ve kapasiteyi al
            size = other.size;
            capacity = other.capacity;

            // yeni bellek tahsis et ve verileri kopyala
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    Vector<T>& operator+=(const Vector<T>& other) {
        if (size + other.size > capacity) { // eğer kapasite yeterli değilse
            // yeni bir kapasite belirle ve belleği yeniden tahsis et
            capacity = size + other.size;
            T* newData = new T[capacity];
            // eski verileri yeni belleğe kopyala
            for (size_t i = 0; i < size; ++i) {
                newData[i] = data[i];
            }
            // diğer vektörün verilerini yeni belleğe kopyala
            for (size_t i = 0; i < other.size; ++i) {
                newData[size + i] = other.data[i];
            }
            // eski belleği temizle ve yeni belleği ata
            delete[] data;
            data = newData;
            size += other.size;
        } else {
            // kapasite yeterliyse, diğer vektörün verilerini mevcut vektöre ekle
            for (size_t i = 0; i < other.size; ++i) {
                data[size + i] = other.data[i];
            }
            size += other.size;
        }
        return *this;
    }

    T* begin() const {
        return data;
    }

    T* end() const {
        return data + size; 
    }

    size_t getSize() const {
        return size;
    }

    T& operator[](size_t index) {
        if (index >= size) {
            return data[size - 1];
        }
        return data[index];
    }

    void pop_back(size_t index) {
        if (index < size) {
            // Elemanın bellekten serbest bırakılması
            //data[index].~T();

            // Elemanların dizinlerinin yeniden düzenlenmesi
            for (size_t i = index; i < size - 1; ++i) {
                data[i] = Move(data[i + 1]);
            }

            --size;
        }
    }

    Vector<T*> SearchByValue(const T& value) const {
        Vector<T*> result;
        for (auto ptr : data) {
            if (*ptr == value) {
                result.push_back(ptr);
            }
        }
        return result;
    }

    Vector<T> Subvector(T* ptr1, T* ptr2) const {
        Vector<T> result;
        // İşaretçilerin sırasını kontrol et
        const T* first = ptr1 < ptr2 ? ptr1 : ptr2;
        const T* last = ptr1 < ptr2 ? ptr2 : ptr1;
        // İlk işaretçiden başlayarak son işaretçiye kadar olan elemanları ekle
        for (auto it = begin(); it != end(); ++it) {
            if (it >= first && it <= last) {
                result.push_back(*it);
            }
        }
        return result;
    }

    void forEach(void (*callback)(T&)) {
        for (auto& element : data) {
            callback(element);
        }
    }

   List<T> ConvertToList() const {
        List<T> result;
        for (size_t i = 0; i < size; ++i) {
            result.push_back(data[i]);
        }
        return result;
    }
    T* Find(const T& value) const {
        for (size_t i = 0; i < size; ++i) {
            if (data[i] == value) {
                return &data[i];
            }
        }
        return nullptr;
    }   
    int FindIndex(const T& value) const {
        for (size_t i = 0; i < size; ++i) {
            if (data[i] == value) {
                return i;
            }
        }
        return -1;
    }   
};


template <typename T>
class List {
private:

    struct Node {
        T data;
        Node* prev;
        Node* next;

        Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
    };

    Node* head; 
    Node* tail; 
    size_t size;

public:
    List() : head(nullptr), tail(nullptr), size(0) {}

    ~List() {
        clear();
    }

    T& Next(Node* node) {
        return node->next->data;
    }
    T& Prev(Node* node) {
        return node->prev->data;
    }
    void push_front(const T& value) {
        Node* newNode = new Node(value);

        if (empty()) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }

        ++size;
    }

    // Listenin sonuna eleman ekle
    void push_back(const T& value) {
        Node* newNode = new Node(value);

        if (empty()) {
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }

        ++size;
    }

    void pop_front() {
        if (!empty()) {
            Node* oldHead = head;

            if (head == tail) {
                head = tail = nullptr;
            } else {
                head = head->next;
                head->prev = nullptr;
            }

            delete oldHead;
            --size;
        }
    }

    void pop_back() {
        if (!empty()) {
            Node* oldTail = tail;

            if (head == tail) {
                head = tail = nullptr;
            } else {
                tail = tail->prev;
                tail->next = nullptr;
            }

            delete oldTail;
            --size;
        }
    }

     void ShiftRightByIndex(int index) {
        if (index < 0 || index > size) {
            return;
        }

        Node* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }

        // Yeni düğüm ekleniyor (boşluk yaratmak için)
        Node* newNode = new Node(T()); // Varsayılan bir boş değer

        if (current == nullptr) {
            // Eğer current null ise (listenin sonundaysa), yeni düğüm sona eklenir.
            if (tail != nullptr) {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            } else {
                head = tail = newNode;
            }
        } else {
            // Eğer current bir düğümü işaret ediyorsa, onu sağa kaydır.
            newNode->next = current;
            newNode->prev = current->prev;

            if (current->prev != nullptr) {
                current->prev->next = newNode;
            } else {
                head = newNode;
            }

            current->prev = newNode;
        }

        ++size;
    }

    size_t getSize() const {
        return size;
    }


    bool empty() const {
        return size == 0;
    }

    void clear() {
        while (!empty()) {
            pop_front();
        }
    }

    class Iterator {
    private:
        Node* current;

    public:
        Iterator(Node* node) : current(node) {}

        T& operator*() {
            return current->data;
        }

        Iterator& operator++() {
            if (current) {
                current = current->next;
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() {
        return Iterator(head);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    T& operator[](size_t index) {
        if (index >= size) index = size;

        Node* current = head;
        for (size_t i = 0; i < index; ++i) {
            current = current->next;
        }

        return current->data;
    }
    T* Find(const T& value) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->data == value) {
                return &current->data;
            }
            current = current->next;
        }
        return nullptr; 
    }

    Node* FindNode(const T& value) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->data == value) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }   
    int FindIndex(const T& value) const {
        Node* current = head;
        int index = 0;
        while (current != nullptr) {
            if (current->data == value) {
                return index;
            }
            current = current->next;
            ++index;
        }
        return -1;
    }
};



class String {
private:
    char* m_data;
    size_t m_length;
    size_t m_capacity;

public:
    String() : m_data(nullptr), m_length(0), m_capacity(0){}

    String(const char* data) : m_data(nullptr), m_length(0), m_capacity(0) {
        m_length = Strlen(data);
        m_capacity = m_length + 1;
        m_data = new char[m_capacity];
        Strcpy(m_data, data);
    }

    ~String() {
        delete[] m_data;
    }

    String(const String& other) : m_data(nullptr), m_length(0), m_capacity(0) {
        m_length = other.m_length;
        m_capacity = other.m_capacity;
        m_data = new char[m_capacity];
        Strcpy(m_data, other.m_data);
    }

    String& operator=(const String& other) {
        if (this != &other) {
            delete[] m_data;
            m_length = other.m_length;
            m_capacity = other.m_capacity;
            m_data = new char[m_capacity];
            Strcpy(m_data, other.m_data);
        }
        return *this;
    }

    bool operator==(const String& other) const {
        return (Strcmp(m_data, other.m_data) == 0);
    }

    bool operator==(const char* other) const {
        return (Strcmp(m_data, other) == 0);
    } 

    bool operator==(char other) const {
        return (Strcmp(m_data, &other) == 0);
    } 

    bool operator!=(const String& other) const {
        return (Strcmp(m_data, other.m_data) != 0);
    }

    static const size_t npos = 0xFFFFFFFFFFFFFFFF;

    size_t find(const char* substr) const {
        if (m_data == nullptr || substr == nullptr)
            return npos; // Return npos if either string or substring is nullptr
        const char* result = Strstr(m_data, substr);
        if (result == nullptr)
            return npos; // Return npos if substring is not found in string
        return result - m_data; // Return index of first occurrence of substring
    }

    bool isEmpty() {
        return m_data == nullptr || *m_data == '\0';
    }

    void clear() {
        delete[] m_data;
        m_data = new char[1];
        m_data[0] = '\0';
        m_length = 0;
    }

    size_t size() {
        size_t length = 0;
        while (m_data[length] != '\0') {
            length++;
        }
        return length;
    }

    bool isNumeric() {
    // Boş string kontrolü
    if (isEmpty()) {
        return false;
    }

    // İlk karakter kontrolü (+ veya - işareti olabilir)
    size_t i = 0;
    /*if (m_data[i] == '+' || m_data[i] == '-') {
        ++i;
    }*/

    // Geri kalan karakterlerin hepsinin rakam olup olmadığını kontrol et
    for (; i < size(); ++i) {
        if (!isDigit(m_data[i])) {
            return false;
        }
    }

    return true;
}

    String operator+(const String& other) const {
        size_t newLength = m_length + other.m_length;
        char* newData = new char[newLength + 1];
        
        Strcpy(newData, m_data);
        Strcat(newData, other.m_data);
        
        return String(newData);
    }

    String& operator+=(const String& other) {
        size_t newLength = m_length + other.m_length;
        char* newData = new char[newLength + 1];
        
        Strcpy(newData, m_data);
        Strcat(newData, other.m_data);
        
        delete[] m_data;
        m_data = newData;
        m_length = newLength;
        
        return *this;
    }

    String& operator+=(char other) {
        size_t newLength = m_length + 1;
        char* newData = new char[newLength + 1];
        
        Strcpy(newData, m_data);
        Strcat(newData, &other);
        
        delete[] m_data;
        m_data = newData;
        m_length = newLength;
        
        return *this;
    }

    const char& operator[](size_t index) const {
        if (index >= m_length) {
            return m_data[m_length];
        }
        return m_data[index];
    }

    size_t find_first_of(const char* str, size_t pos = 0) const {
        if (pos >= m_length) {
            return npos; // Arama konumu geçerli dizenin sınırını aşarsa, bulunamadı döndür
        }

        const char* found = Strpbrk(m_data + pos, str);
        if (found == nullptr) {
            return npos; // Belirtilen karakterler bulunamazsa, bulunamadı döndür
        }

        return found - m_data; // Bulunan karakterin dizindeki konumunu döndür
    }

    void insert(size_t index, char* data) {
        size_t newDataLength = Strlen(data);
        if (index > m_length)
            return;
        if (m_length + newDataLength >= m_capacity) {
            // Reallocate memory if necessary
            m_capacity = m_length + newDataLength + 1;
            char* newData = new char[m_capacity];
            Strcpy(newData, m_data);
            delete[] m_data;
            m_data = newData;
        }
        Memmove(m_data + index + newDataLength, m_data + index, m_length - index + 1);
        // Copy new data into the appropriate position
        Memcpy(m_data + index, data, newDataLength);
        m_length += newDataLength;
    }

    void push_back(char ch) {
        if (m_length + 1 >= m_capacity) {
            // Reallocate memory if necessary
            m_capacity = m_length * 2 + 1;
            char* newData = new char[m_capacity];
            Strcpy(newData, m_data);
            delete[] m_data;
            m_data = newData;
        }
        // Append character to the end
        m_data[m_length] = ch;
        m_data[++m_length] = '\0';
    }

    String substring(size_t start, size_t length) const {
        char* newData = new char[length + 1];
        Memcpy(newData, m_data + start, length);
        newData[length] = '\0';
        return String(newData);
    }

    size_t length() const {
        return m_length;
    }

    const char* c_str() const {
        return m_data;
    }

    char* begin() {
        return m_data;
    }

    size_t find_first_not_of(const String& characters) const {
        for (size_t i = 0; i < length(); ++i) {
            if (characters.find(m_data + i) == String::npos) {
                return i;
            }
        }
        return String::npos;
    }

    size_t find_last_not_of(const String& characters) const {
        for (size_t i = length() - 1; i >= 0; --i) {
            if (characters.find(m_data + i) == String::npos) {
                return i;
            }
        }
        return String::npos;
    }

    void lstrip() {
        size_t start = find_first_not_of(" \t\n\r\f\v");
        if (start != npos) {
            *this = substring(start, length() - start);
        } else {
            clear(); // Eğer sadece boşluk karakterleri varsa, tamamen boş bir dize yap
        }
    }

    void rstrip() {
        size_t end = find_last_not_of(" \t\n\r\f\v");
        if (end != npos) {
            *this= substring(0, end + 1);
        } else {
            clear(); // Eğer sadece boşluk karakterleri varsa, tamamen boş bir dize yap
        }
    }

    void strip(const String& characters = " \t\n\r\f\v") {
        size_t start = find_first_not_of(characters);
        size_t end = find_last_not_of(characters);

        if (start == npos || end == npos) {
            clear(); // Eğer sadece belirtilen karakterler varsa, tamamen boş bir dize yap
        } else {
            *this = substring(start, end - start + 1);
        }
    }

    
    bool search_at_specific_index(int index, const String& substr) {
        // Geçersiz indeks kontrolü
        if (index < 0 || index >= length()) {
            return false;
        }
        
        // Verilen indexten başlayarak alt dizeyi elde et
        auto sub = substring(index, substr.length());

        // Alt diziyi ve verilen alt dizeyi karşılaştır
        if (sub.length() < substr.length()) {
            return false;
        }

        return sub.compare(substr) == 0;
    }

    bool only_consists_of(const String& characters) const {
        for (size_t i = 0; i < m_length; ++i) {
            if (characters.find(m_data + i) == npos) {
                return false;
            }
        }
        return true;
    }

    int compare(const String& other) const {
        // Bu string'in uzunluğunu ve diğer string'in uzunluğunu al
        size_t len1 = length();
        size_t len2 = other.length();

        // Uzunluklara göre karşılaştırma yap
        size_t minLen = Min(len1, len2);

        for (size_t i = 0; i < minLen; ++i) {
            if (m_data[i] < other.m_data[i]) {
                return -1; // Bu string diğerinden küçük
            } else if (m_data[i] > other.m_data[i]) {
                return 1; // Bu string diğerinden büyük
            }
        }

        // Uzunluklar eşitse
        if (len1 < len2) {
            return -1; // Bu string diğerinden küçük
        } else if (len1 > len2) {
            return 1; // Bu string diğerinden büyük
        }

        return 0; // İki string de eşit
    }

   /*Vector<String> tokenize(char delimiter, const String& specials){
        
        Vector<String> Result;
        Vector<String> SpecialChars = specials.split(' ');
        Vector<int> Positions;

        int start = 0;
        DEBUG("Entering loop!");
        for (int i = 0; i < m_length; i++) {
            for (auto character: SpecialChars) {
                if (search_at_specific_index(i, character)) {
                    Positions.push_back(i);
                    Positions.push_back(i+ character.length());
                }
            }
        }
        auto PosSize = Positions.getSize();
        for (int i = 0; i < PosSize; i++) Positions[i] += i;

        for (auto pos: Positions) insert(pos, " ");

        DEBUG(m_data);
        DEBUG(this);
        Result = split(' ');

        for (auto res: Result) DEBUG(res.c_str());

        return Result;
    }*/
      

    Vector<String> split(char delimiter) const {
        // Geçici bir dizi oluştur
        
        Vector<String> result;
        Vector<int> poses;

        size_t start = 0;

        // Diziyi ayraç karakterine göre bölmek
        for (size_t i = 0; i < m_length; ++i) {
            if (m_data[i] == delimiter) {
                result.push_back(substring(start, i - start));
                start = i + 1;
            }
        }

        // Son parçayı ekleyin
        
        
        result.push_back(substring(start, m_length - start)); 

        int j = 0;

        for (auto word : result) {
            if (word.only_consists_of(" "))
                poses.push_back(j);
            j++;
        }

        for (int k = 0; k < poses.getSize(); k++) {
            poses[k] -= k;
        }

        for (auto pos : poses) result.pop_back(pos);

        return result;
    }
};

/*String ToString(int value) {
    String result;

    if (value == 0) {
        result.push_back('0');
        return result;
    }

    if (value < 0) {
        result.push_back('-');
        value = -value;
    }

    while (value != 0) {
        char digit = '0' + (value % 10);
        result.insert(0, &digit);
        value /= 10;
    }

    return result;
    
}
*/
template<typename T>
class Optional {
private:
    bool m_hasValue;
    T m_value;

public:
    // Default constructor
    Optional() : m_hasValue(false) {}

    Optional(bool novalue) : m_hasValue(false) {}

    // Value constructor
    Optional(const T& value) : m_hasValue(true), m_value(value) {}

    // Copy constructor
    Optional(const Optional& other) : m_hasValue(other.m_hasValue), m_value(other.m_value) {}

    // Destructor
    ~Optional() {}

    // Assignment operator
    Optional& operator=(const Optional& other) {
        m_hasValue = other.m_hasValue;
        m_value = other.m_value;
        return *this;
    }

    // Value setter
    void set(const T& value) {
        m_value = value;
        m_hasValue = true;
    }

    // Value getter
    T get() const {
        if (!m_hasValue) {
            return *new T;
        }
        return m_value;
    }

    // Check if value is set
    bool hasValue() const {
        return m_hasValue;
    }

    // Clear value
    void clear() {
        m_hasValue = false;
    }
};



}