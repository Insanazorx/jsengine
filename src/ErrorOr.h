#pragma once

#include <iostream>
#include <string>
#include "Debug.h"
#include <utility>

// Basit ErrorOr sınıfı. Bir T değeri veya hata mesajı barındırır.
namespace Util {
    template <typename T>
    concept ErrorConcept = requires (T t)
    {
        {t.what()} -> std::string;
    };

    class Error {
    public:

        explicit Error(std::string what) : m_what(std::move(what)) {}

        virtual ~Error() = default;
        [[nodiscard]] std::string what() const { return m_what; }
    protected:
        std::string m_what;
    };

    template<typename T, ErrorConcept E>
    class ErrorOr {
    private:

        explicit ErrorOr(const T& value) : m_ok(true), m_value(value), m_error(E("no error")) {}
        explicit ErrorOr(T&& value) : m_ok(true), m_value(std::move(value)), m_error(E("no error")) {}
        explicit ErrorOr(E& error) : m_ok(false), m_value(), m_error(std::move(error)) {}

    public:
        ~ErrorOr() = default;
        // Başarılı durumda T değerini sarmallar.
        static ErrorOr Ok(const T& value) {
            return ErrorOr (value);
        }

        operator bool() const { return m_ok; }


        static ErrorOr Ok(T&& value) {
            return ErrorOr(std::move(value));
        }


        // Hata durumunu sarmalar.
        static ErrorOr Err(E err)
        {
            return ErrorOr(std::move(err));
        }

        // Durum kontrolü.
        bool ok() const { return m_ok; }

        // Başarılı durumda değeri döndürür.
        // Hata durumunda çağırılmamalıdır.
        const T& value() const { return m_value; }
        T& value() { return m_value; }

        // Hata mesajını döndürür.
        const E& error() const { return m_error; }

    private:
        bool m_ok;
        T m_value;
        E m_error;
    };

    // void için özel şablonlandırma (opsiyonel)
    // Eğer void dönen işlemler için ErrorOr kullanmak istersen:
    template<ErrorConcept E>
    class ErrorOr<void, E> {
    private:
        explicit ErrorOr(const E& error) : m_ok(false), m_error(error) {}
        explicit ErrorOr(E& error) : m_ok(false), m_error(std::move(error)) {}
    public:
        ~ErrorOr() = default;

        static ErrorOr Err(const E& error) {
            return ErrorOr(error);
        }

        bool ok() const { return m_ok;}
        const E error() const { return m_error; }

    private:
        bool m_ok;
        E m_error;
    };


#define TRY(expr)                                                   \
    ([&]() -> decltype(expr) {                                      \
        auto _result = (expr);                                      \
        if (!_result.ok()) {                                        \
            return decltype(_result)::Err(_result.error());         \
        }                                                           \
            return decltype(_result)::Ok(_result.value());          \
        }());


#define TRY_FOR_VOID(expr)                                          \
    ([&]() {                                               \
        auto _result = (expr);                                      \
        if (!_result.ok()) {                                        \
            return;                                 \
        }                                                           \
        return decltype(expr)::Err(_result.error());                                   \
    }());
};