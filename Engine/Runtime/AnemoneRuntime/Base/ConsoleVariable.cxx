#include "AnemoneRuntime/Base/ConsoleVariable.hxx"

namespace Anemone
{
    void ConsoleVariableRegistry::Register(IConsoleVariable* variable)
    {
        this->m_variables.PushBack(variable);
    }

    void ConsoleVariableRegistry::Unregister(IConsoleVariable* variable)
    {
        this->m_variables.Remove(variable);
    }

    void ConsoleVariableRegistry::Enumerate(FunctionRef<void(IConsoleVariable&)> callback)
    {
        this->m_variables.ForEach([&callback](IConsoleVariable& variable)
        {
            callback(variable);
        });
    }

    IConsoleVariable* ConsoleVariableRegistry::FindByName(std::string_view name) const
    {
        return this->m_variables.Find([&name](IConsoleVariable const& variable)
        {
            return variable.Name() == name;
        });
    }

    ConsoleVariableRegistry& ConsoleVariableRegistry::Get()
    {
        static ConsoleVariableRegistry instance;
        return instance;
    }
}
