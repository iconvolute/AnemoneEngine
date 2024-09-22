#pragma once

namespace Anemone
{
    template <typename T>
    class Delegate;

    template <typename R, typename... Ts>
    class Delegate<R(Ts...)>
    {
    private:
        using CallbackType = R(void*, Ts...);

    private:
        void* _object;
        CallbackType* _callback;

    private:
        Delegate(void* object, CallbackType* callback)
            : _object{object}
            , _callback{callback}
        {
        }

    public:
        Delegate() = default;

        constexpr bool operator==(const Delegate& other) const
        {
            return (this->_object == other._object) && (this->_callback == other._callback);
        }

        constexpr bool operator!=(const Delegate& other) const
        {
            return !(*this == other);
        }

        constexpr explicit operator bool() const
        {
            return this->_callback != nullptr;
        }

        constexpr R Invoke(Ts... args) const
        {
            return this->_callback(this->_object, std::forward<Ts>(args)...);
        }

        constexpr R operator()(Ts... args) const
        {
            return this->_callback(this->_object, std::forward<Ts>(args)...);
        }

        template <auto FunctionT>
        static constexpr Delegate FromFunction()
        {
            return Delegate{nullptr, [](void*, Ts... args)
            {
                return FunctionT(args...);
            }};
        }

        template <auto Method, typename Self>
        static constexpr Delegate FromMethod(Self* self)
        {
            return Delegate{self, [](void* object, Ts... args)
            {
                return (static_cast<Self*>(object)->*Method)(args...);
            }};
        }

        template <typename Lambda>
        static constexpr Delegate FromLambda(Lambda lambda)
        {
            return Delegate{
                &lambda, [](void* object, Ts... args)
            {
                return (*static_cast<Lambda*>(object))(args...);
            }};
        }
    };
}
