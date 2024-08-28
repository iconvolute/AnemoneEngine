#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/CommandLine.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

namespace Anemone
{
    class IConsoleFunction;

    class ConsoleFunctionRegistry final
    {
    private:
        IntrusiveList<IConsoleFunction, ConsoleFunctionRegistry> m_functions{};

    public:
        ConsoleFunctionRegistry() = default;
        ConsoleFunctionRegistry(ConsoleFunctionRegistry const&) = delete;
        ConsoleFunctionRegistry(ConsoleFunctionRegistry&&) = delete;
        ConsoleFunctionRegistry& operator=(ConsoleFunctionRegistry const&) = delete;
        ConsoleFunctionRegistry& operator=(ConsoleFunctionRegistry&&) = delete;
        ~ConsoleFunctionRegistry() = default;

    public:
        RUNTIME_API void Register(IConsoleFunction* function);

        RUNTIME_API void Unregister(IConsoleFunction* function);

        RUNTIME_API void Enumerate(FunctionRef<void(IConsoleFunction&)> callback);

        RUNTIME_API IConsoleFunction* FindByName(std::string_view name) const;

        RUNTIME_API static ConsoleFunctionRegistry& Get();
    };

    class IConsoleFunction
        : private IntrusiveListNode<IConsoleFunction, ConsoleFunctionRegistry>
    {
        friend struct IntrusiveList<IConsoleFunction, ConsoleFunctionRegistry>;

    private:
        std::string_view m_name{};

    public:
        IConsoleFunction(std::string_view name)
            : m_name{name}
        {
            ConsoleFunctionRegistry::Get().Register(this);
        }

        IConsoleFunction(IConsoleFunction const&) = delete;
        IConsoleFunction(IConsoleFunction&&) = delete;
        IConsoleFunction& operator=(IConsoleFunction const&) = delete;
        IConsoleFunction& operator=(IConsoleFunction&&) = delete;

        virtual ~IConsoleFunction()
        {
            ConsoleFunctionRegistry::Get().Unregister(this);
        }

        [[nodiscard]] std::string_view Name() const
        {
            return this->m_name;
        }

        virtual void Execute(CommandlineParser const& arguments) = 0;
    };

    template <typename CallbackT = void(CommandlineParser const&)>
    class ConsoleFunction final : public IConsoleFunction
    {
    private:
        CallbackT m_callback;

    public:
        explicit ConsoleFunction(std::string_view name, CallbackT&& callback)
            : IConsoleFunction{name}
            , m_callback{std::forward<CallbackT>(callback)}
        {
        }

        void Execute(CommandlineParser const& arguments) override
        {
            this->m_callback(arguments);
        }
    };
}
