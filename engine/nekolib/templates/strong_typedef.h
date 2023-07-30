#pragma once
#include "engine/nekolib/templates/concepts.h"

/**
 * Creates a type that is not just an alias (for templates) which means
 * std::is_same_v<T, NT> is false. This is different from using/typedef
 * keywords. The new type behaves exactly the same as the old one.
 *
 * TODO: does not work yet, if T is not buildable ...
 * TODO: remove template once concepts for non-template methods is added
 */
#define KODANUKI_STRONG_TYPEDEF(T, NT)                                           \
                                                                                 \
    namespace                                                                    \
    {                                                                            \
                                                                                 \
    template <int dummy>                                                         \
    struct _##NT : public T                                                      \
    {                                                                            \
        _##NT operator=(const T& v) requires std::copy_constructible<T> {        \
            *this = v; return *this;                                             \
        }                                                                        \
        _##NT(const T& v) requires std::copy_constructible<T> : T(v) {}          \
                                                                                 \
        _##NT operator=(T&& v) requires std::move_constructible<T> {             \
            *this = _##NT(std::move(v)); return *this;                           \
        }                                                                        \
        _##NT(T&& v) requires std::move_constructible<T> : T(std::move(v)) {}    \
                                                                                 \
        static _##NT create(typename T::Builder builder) requires buildable<T> { \
            return _##NT(T::create(builder));                                    \
        }                                                                        \
    };                                                                           \
                                                                                 \
    }                                                                            \
                                                                                 \
    using NT = _##NT<0>
