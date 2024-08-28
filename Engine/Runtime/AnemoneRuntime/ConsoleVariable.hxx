#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/CommandLine.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

namespace Anemone
{
    class IConsoleVariable;

    class ConsoleVariableRegistry final
    {
    private:
        IntrusiveList<IConsoleVariable, ConsoleVariableRegistry> m_variables{};

    public:
        ConsoleVariableRegistry() = default;
        ConsoleVariableRegistry(ConsoleVariableRegistry const&) = delete;
        ConsoleVariableRegistry(ConsoleVariableRegistry&&) = delete;
        ConsoleVariableRegistry& operator=(ConsoleVariableRegistry const&) = delete;
        ConsoleVariableRegistry& operator=(ConsoleVariableRegistry&&) = delete;
        ~ConsoleVariableRegistry() = default;

    public:
        RUNTIME_API void Register(IConsoleVariable* variable);

        RUNTIME_API void Unregister(IConsoleVariable* variable);

        RUNTIME_API void Enumerate(FunctionRef<void(IConsoleVariable&)> callback);

        RUNTIME_API IConsoleVariable* FindByName(std::string_view name) const;

        RUNTIME_API static ConsoleVariableRegistry& Get();
    };

    class IConsoleVariable
        : private IntrusiveListNode<IConsoleVariable, ConsoleVariableRegistry>
    {
        friend struct IntrusiveList<IConsoleVariable, ConsoleVariableRegistry>;

    private:
        std::string_view m_name{};

    public:
        IConsoleVariable(std::string_view name)
            : m_name{name}
        {
            ConsoleVariableRegistry::Get().Register(this);
        }

        IConsoleVariable(IConsoleVariable const&) = delete;
        IConsoleVariable(IConsoleVariable&&) = delete;
        IConsoleVariable& operator=(IConsoleVariable const&) = delete;
        IConsoleVariable& operator=(IConsoleVariable&&) = delete;

        virtual ~IConsoleVariable()
        {
            ConsoleVariableRegistry::Get().Unregister(this);
        }

        [[nodiscard]] std::string_view Name() const
        {
            return this->m_name;
        }
    };

    template <typename T>
    class ConsoleVariable final : public IConsoleVariable
    {
    private:
        T m_value{};

    public:
        explicit ConsoleVariable(std::string_view name)
            : IConsoleVariable{name}
        {
        }

        explicit ConsoleVariable(std::string_view name, T const& value)
            : IConsoleVariable{name}
            , m_value{value}
        {
        }

        [[nodiscard]] T const& Get() const
        {
            return this->m_value;
        }

        void Set(T const& value)
        {
            this->m_value = value;
        }
    };
}
