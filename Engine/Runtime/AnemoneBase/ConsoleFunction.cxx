#include "AnemoneBase/ConsoleFunction.hxx"

namespace Anemone
{
    void ConsoleFunctionRegistry::Register(IConsoleFunction* function)
    {
        UniqueLock scope{this->m_lock};

        this->m_functions.PushBack(function);
    }

    void ConsoleFunctionRegistry::Unregister(IConsoleFunction* function)
    {
        UniqueLock scope{this->m_lock};

        this->m_functions.Remove(function);
    }

    void ConsoleFunctionRegistry::Enumerate(FunctionRef<void(IConsoleFunction&)> callback)
    {
        UniqueLock scope{this->m_lock};

        this->m_functions.ForEach([&callback](IConsoleFunction& function)
        {
            callback(function);
        });
    }

    IConsoleFunction* ConsoleFunctionRegistry::FindByName(std::string_view name) const
    {
        UniqueLock scope{this->m_lock};

        return this->m_functions.Find([&name](IConsoleFunction const& function)
        {
            return function.Name() == name;
        });
    }

    ConsoleFunctionRegistry& ConsoleFunctionRegistry::Get()
    {
        static ConsoleFunctionRegistry instance;
        return instance;
    }
}
