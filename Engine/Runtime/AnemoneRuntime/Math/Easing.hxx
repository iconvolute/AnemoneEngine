#pragma once
#include "AnemoneRuntime/Math/Functions.hxx"

namespace Anemone::Math::Easing
{
    template <typename T>
    T EaseInQuad(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * t * t + start;
    }

    template <typename T>
    T EaseOutQuad(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return -end * t * (t - T(2)) + start;
    }

    template <typename T>
    T EaseInOutQuad(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t *= T(2);
        end -= start;

        if (t < T(1))
        {
            return end * T(0.5) * t * t + start;
        }

        t -= T(1);

        return -end * T(0.5) * (t * (t - T(2)) - T(1)) + start;
    }

    template <typename T>
    T EaseInCubic(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * t * t * t + start;
    }

    template <typename T>
    T EaseOutCubic(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t -= T(1);
        end -= start;
        return end * (t * t * t + T(1)) + start;
    }

    template <typename T>
    T EaseInOutCubic(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t *= T(2);
        end -= start;

        if (t < T(1))
        {
            return end * T(0.5) * t * t * t + start;
        }

        t -= T(2);
        return end * T(0.5) * (t * t * t + T(2)) + start;
    }

    template <typename T>
    T EaseInQuart(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * t * t * t * t + start;
    }

    template <typename T>
    T EaseOutQuart(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t -= T(1);
        end -= start;
        return -end * (t * t * t * t - T(1)) + start;
    }

    template <typename T>
    T EaseInOutQuart(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t *= T(2);
        end -= start;

        if (t < T(1))
        {
            return end * T(0.5) * t * t * t * t + start;
        }

        t -= T(2);
        return -end * T(0.5) * (t * t * t * t - T(2)) + start;
    }

    template <typename T>
    T EaseInQuint(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * t * t * t * t * t + start;
    }

    template <typename T>
    T EaseOutQuint(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t -= T(1);
        end -= start;
        return end * (t * t * t * t * t + T(1)) + start;
    }

    template <typename T>
    T EaseInOutQuint(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t *= T(2);
        end -= start;

        if (t < T(1))
        {
            return end * T(0.5) * t * t * t * t * t + start;
        }

        t -= T(2);
        return end * T(0.5) * (t * t * t * t * t + T(2)) + start;
    }

    template <typename T>
    T EaseInSine(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return -end * Cos<T>(t * (Pi<T> * T(0.5))) + end + start;
    }

    template <typename T>
    T EaseOutSine(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * Sin<T>(t * (Pi<T> * T(0.5))) + start;
    }

    template <typename T>
    T EaseInOutSine(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return -end * T(0.5) * (Cos<T>(Pi<T> * t) - T(1)) + start;
    }

    template <typename T>
    T EaseInExpo(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * Power<T>(T(2), T(10) * (t - T(1))) + start;
    }

    template <typename T>
    T EaseOutExpo(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * (-Power<T>(T(2), T(-10) * t) + T(1)) + start;
    }

    template <typename T>
    T EaseInOutExpo(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t *= T(2);
        end -= start;

        if (t < T(1))
        {
            return end * T(0.5) * Power<T>(T(2), T(10) * (t - T(1))) + start;
        }

        t -= T(1);
        return end * T(0.5) * (-Power<T>(T(2), T(-10) * t) + T(2)) + start;
    }

    template <typename T>
    T EaseInCirc(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return -end * (Sqrt<T>(T(1) - t * t) - T(1)) + start;
    }

    template <typename T>
    T EaseOutCirc(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t -= T(1);
        end -= start;
        return end * Sqrt<T>(T(1) - t * t) + start;
    }

    template <typename T>
    T EaseInOutCirc(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        t *= T(2);
        end -= start;

        if (t < T(1))
        {
            return -end * T(0.5) * (Sqrt<T>(T(1) - t * t) - T(1)) + start;
        }

        t -= T(2);

        return end * T(0.5) * (Sqrt<T>(T(1) - t * t) + T(1)) + start;
    }

    template <typename T>
    T EaseOutBounce(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        //?? t /= T(1);
        end -= start;

        if (t < T(1.0 / 2.75))
        {
            return end * (T(7.5625) * t * t) + start;
        }

        if (t < T(2.0 / 2.75))
        {
            t -= T(1.5 / 2.75);
            return end * (T(7.5625) * t * t + T(0.75)) + start;
        }

        if (t < T(2.5 / 2.75))
        {
            t -= T(2.25 / 2.75);
            return end * (T(7.5625) * t * t + T(0.9375)) + start;
        }

        t -= T(2.625 / 2.75);
        return end * (T(7.5625) * t * t + T(0.984375)) + start;
    }

    template <typename T>
    T EaseInBounce(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end - EaseOutBounce(T(0), end, T(1) - t) + start;
    }

    template <typename T>
    T EaseInOutBounce(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;

        if (t < T(0.5))
        {
            return EaseInBounce<T>(T(0), end, t * T(2)) * T(0.5) + start;
        }

        return EaseOutBounce<T>(T(0), end, t * T(2) - T(1)) * T(0.5) + end * T(0.5) + start;
    }

    template <typename T>
    T EaseInBack(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        static constexpr T s = T(1.70158);

        end -= start;

        return end * t * t * ((s + T(1)) * t - s) + start;
    }

    template <typename T>
    T EaseOutBack(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        static constexpr T s = T(1.70158);

        end -= start;
        t -= T(1);
        return end * (t * t * ((s + T(1)) * t + s) + T(1)) + start;
    }

    template <typename T>
    T EaseInOutBack(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        static constexpr T s = T(1.70158 * 1.525);

        end -= start;
        t *= T(2);

        if (t < T(1))
        {
            return end * T(0.5) * (t * t * ((s + T(1)) * t - s)) + start;
        }

        t -= T(2);
        return end * T(0.5) * (t * t * ((s + T(1)) * t + s) + T(2)) + start;
    }

    template <typename T>
    T EaseInElastic(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;

        T d = T(1);
        T p = d * T(0.3);
        T s;
        T a = T(0);

        if (t == T(0))
        {
            return start;
        }

        if ((t /= d) == T(1))
        {
            return start + end;
        }

        if ((a == T(0)) or (a < Abs(end)))
        {
            a = end;
            s = p * T(0.25);
        }
        else
        {
            s = p / (T(2) * Pi<T>)*ASin<T>(end / a);
        }

        t -= T(1);

        return -(a * Power<T>(T(2), T(10) * t) * Sin<T>((t * d - s) * (T(2) * Pi<T>) / p)) + start;
    }

    template <typename T>
    T EaseOutElastic(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;

        T d = T(1);
        T p = d * T(0.3);
        T s;
        T a = T(0);

        if (t == T(0))
        {
            return start;
        }

        if ((t /= d) == T(1))
        {
            return start + end;
        }

        if ((a == T(0)) or (a < Abs<T>(end)))
        {
            a = end;
            s = p * T(0.25);
        }
        else
        {
            s = p / (T(2) * Pi<T>)*ASin<T>(end / a);
        }

        return a * Power<T>(T(2), -T(10) * t) * Sin<T>((t * d - s) * (T(2) * Pi<T>) / p) + end + start;
    }

    template <typename T>
    T EaseInOutElastic(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;

        T d = T(1);
        T p = d * T(0.3);
        T s;
        T a = T(0);

        if (t == T(0))
        {
            return start;
        }

        if ((t /= d * T(0.5)) == T(2))
        {
            return start + end;
        }

        if ((a == T(0)) or (a < Abs(end)))
        {
            a = end;
            s = p * T(0.25);
        }
        else
        {
            s = p / (T(2) * Pi<T>)*ASin<T>(end / a);
        }


        if (t < T(1))
        {
            t -= T(1);
            return -T(0.5) * (a * Power<T>(T(2), T(10) * t) * Sin<T>((t * d - s) * (T(2) * Pi<T>) / p)) + start;
        }

        t -= T(1);
        return a * Power<T>(T(2), -T(10) * t) * Sin<T>((t * d - s) * (T(2) * Pi<T>) / p) * T(0.5) + end + start;
    }
}

namespace Anemone::Math::Easing
{
    template <typename T>
    T EaseSmoothC1(T t)
        requires(std::is_floating_point_v<T>)
    {
        return t * t * (T(3) - T(2) * t);
    }

    template <typename T>
    T EaseSmoothC1(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * EaseSmoothC1(t) + start;
    }

    template <typename T>
    T EaseSmoothC2(T t)
        requires(std::is_floating_point_v<T>)
    {
        return t * t * t * (t * (t * T(6) - T(15)) + T(10));
    }

    template <typename T>
    T EaseSmoothC2(T start, T end, T t)
        requires(std::is_floating_point_v<T>)
    {
        end -= start;
        return end * EaseSmoothC2(t) + start;
    }

    // EaseInCubic
    template <typename T>
    T EaseSmoothCubed(T start, T end, T t)
    {
        T const a = t * t * t;
        return (end * a) + (start * (T(1) - a));
    }

    // EaseOutCubic
    template <typename T>
    T SmoothInvCubed(T max, T min, T value) noexcept
    {
        const auto n = T(1) - value;
        const auto a = T(1) - n * n * n;
        return (min * a) + (max * (T(1) - a));
    }

    // EaseOutQuad
    template <typename Type>
    Type SmoothInvSquared(const Type& max, const Type& min, const Type& x)
    {
        Type _x = 1 - (1 - x) * (1 - x);
        return (min * _x) + (max * (1 - _x));
    }


}
