#pragma once
#include "AnemoneMath/Functions.hxx"

namespace Anemone
{
    template <std::floating_point T>
    class PidController final
    {
    private:
        T _previousError{};
        T _integralAccum{};
        T _kp{};
        T _ki{};
        T _kd{};
        T _minValue{};
        T _maxValue{};

    public:
        explicit constexpr PidController(T kp = {}, T ki = {}, T kd = {})
            : _kp{kp}
            , _ki{ki}
            , _kd{kd}
            , _minValue{std::numeric_limits<T>::min()}
            , _maxValue{std::numeric_limits<T>::max()}
        {
        }

        explicit constexpr PidController(T minValue, T maxValue, T kp = {}, T ki = {}, T kd = {})
            : _kp{kp}
            , _ki{ki}
            , _kd{kd}
            , _minValue{minValue}
            , _maxValue{maxValue}
        {
        }

        constexpr T Evaluate(T error, T delta)
        {
            T const actualError = error * delta;
            this->_integralAccum += actualError;

            T const action = (this->_kp * error) + (this->_ki * this->_integralAccum) + (this->_kd * (error - this->_previousError) / delta);
            T const clamped = Clamp<T>(action, this->_minValue, this->_maxValue);

            if (clamped != action)
            {
                this->_integralAccum -= actualError;
            }

            this->_previousError = error;
            return action;
        }

        constexpr void Reset()
        {
            this->_previousError = {};
            this->_integralAccum = {};
        }
    };
}
