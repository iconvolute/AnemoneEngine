#pragma once
#include <type_traits>
#include <concepts>
#include <limits>

namespace Anemone
{
    template <std::integral T>
    [[nodiscard]] constexpr bool CheckedAdd(const T left, const T right, T& result)
    {
#ifdef __clang__
        if constexpr (std::integral<T>)
        {
            return __builtin_add_overflow(left, right, &result);
        }
        else
#endif // __clang__
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                result = static_cast<T>(left + right);
                return result < left || result < right;
            }
            else
            {
                using U = std::make_unsigned_t<T>;
                result = static_cast<T>(static_cast<U>(left) + static_cast<U>(right));
                return (left > 0 && right > 0 && result <= 0) || (left < 0 && right < 0 && result >= 0);
            }
        }
    }

    template <std::integral T>
    [[nodiscard]] constexpr bool CheckedSub(const T left, const T right, T& result)
    {
#ifdef __clang__
        if constexpr (std::integral<T>)
        {
            return __builtin_sub_overflow(left, right, &result);
        }
        else
#endif // __clang__
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                result = static_cast<T>(left - right);
                return result > left;
            }
            else
            {
                using U = std::make_unsigned_t<T>;
                result = static_cast<T>(static_cast<U>(left) - static_cast<U>(right));
                return (left <= 0 && right > 0 && result >= 0) || (left >= 0 && right < 0 && result <= 0);
            }
        }
    }

    template <std::integral T>
    [[nodiscard]] constexpr bool CheckedMul(const T left, const T right, T& result)
    {
#ifdef __clang__
        if constexpr (std::integral<T>)
        {
            return __builtin_mul_overflow(left, right, &result);
        }
        else
#endif // __clang__
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                result = static_cast<T>(left * right);
                return left != 0 && right > (std::numeric_limits<T>::max)() / left;
            }
            else
            {
                // https://github.com/llvm/llvm-project/blob/88e5206/llvm/include/llvm/Support/MathExtras.h#L725-L750

                //===----------------------------------------------------------------------===//
                //
                // Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
                // See https://llvm.org/LICENSE.txt for license information.
                // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
                //
                //===----------------------------------------------------------------------===//
                using U = std::make_unsigned_t<T>;
                const U unsigned_left = left < 0 ? (0 - static_cast<U>(left)) : static_cast<U>(left);
                const U unsigned_right = right < 0 ? (0 - static_cast<U>(right)) : static_cast<U>(right);
                const U unsigned_result = static_cast<U>(unsigned_left * unsigned_right);

                const bool negative = (left < 0) != (right < 0);
                result = static_cast<T>(negative ? (0 - unsigned_result) : unsigned_result);
                if (unsigned_left == 0 || unsigned_right == 0)
                {
                    return false;
                }

                if (negative)
                {
                    return unsigned_left > (static_cast<U>((std::numeric_limits<T>::max)()) + U{1}) / unsigned_right;
                }
                else
                {
                    return unsigned_left > static_cast<U>((std::numeric_limits<T>::max)()) / unsigned_right;
                }
                // Based on llvm::MulOverflow
            }
        }
    }
}
