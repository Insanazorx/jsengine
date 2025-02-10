#pragma once

#include <iostream>
#include <string>
#include "Debug.h"
#include <utility>

// Basit ErrorOr sınıfı. Bir T değeri veya hata mesajı barındırır.
namespace Util {
    class Error {
    public:

        Error(std::string what) : m_what(std::move(what)) {}

        virtual ~Error() = default;
        std::string what() const { return m_what; }
    private:
        std::string m_what;
    };

    template<typename T>
    class ErrorOr {
    private:

        explicit ErrorOr(const T& value) : m_ok(true), m_value(value), m_error(Error("no error")) {}
        explicit ErrorOr(T&& value) : m_ok(true), m_value(std::move(value)), m_error(Error("no error")) {}
        explicit ErrorOr(Error& error) : m_ok(false), m_value(), m_error(std::move(error)) {}

    public:
        ~ErrorOr() = default;
        // Başarılı durumda T değerini sarmallar.
        static ErrorOr<T> Ok(const T& value) {
            return ErrorOr<T>(value);
        }


        static ErrorOr<T> Ok(T&& value) {
            return ErrorOr<T>(std::move(value));
        }


        // Hata durumunu sarmalar.
        static ErrorOr<T> Err(Error err)
        {
            return ErrorOr<T>(std::move(err));
        }

        // Durum kontrolü.
        bool ok() const { return m_ok; }

        // Başarılı durumda değeri döndürür.
        // Hata durumunda çağırılmamalıdır.
        const T& value() const { return m_value; }
        T& value() { return m_value; }

        // Hata mesajını döndürür.
        const Error& error() const { return m_error; }

    private:
        bool m_ok;
        T m_value;
        Error m_error;
    };

    // void için özel şablonlandırma (opsiyonel)
    // Eğer void dönen işlemler için ErrorOr kullanmak istersen:
    template<>
    class ErrorOr<void> {
    private:
        explicit ErrorOr(const Error& error) : m_ok(false), m_error(error) {}
        explicit ErrorOr(Error& error) : m_ok(false), m_error(std::move(error)) {}
    public:
        ~ErrorOr() = default;

        static ErrorOr<void> Err(const Error& error) {
            return ErrorOr<void>(error);
        }

        bool ok() const { return m_ok;}
        const Error error() const { return m_error; }

    private:
        bool m_ok;
        Error m_error;
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
        return ErrorOr<void>::Err(_result.error());                                   \
    }());
};