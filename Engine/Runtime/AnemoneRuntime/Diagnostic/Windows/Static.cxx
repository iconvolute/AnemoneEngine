#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputLogListener.hxx"

namespace Anemone::Diagnostic
{
    class DebugOutputLogListener : public LogListener
    {
    public:
        void Log(Diagnostic::LogLevel level, std::string_view message) override
        {
            (void)level;
            Platform::win32_string_buffer<wchar_t, 512> buffer{};
            Platform::win32_WidenString(buffer, message);
            OutputDebugStringW(buffer.data());
        }

        void Flush() override
        {
        }

        DebugOutputLogListener()
        {
            Diagnostic::AddLogListener(this);
        }

        ~DebugOutputLogListener() override
        {
            Diagnostic::RemoveLogListener(this);
        }
    };

    static UninitializedObject<DebugOutputLogListener> GDebugOutputLogListener{};
    static UninitializedObject<StandardOutputLogListener> GStandardOutputLogListener{};

    void InitializeStatic()
    {
        GDebugOutputLogListener.Create();
        GStandardOutputLogListener.Create();
    }

    void ShutdownStatic()
    {
        GStandardOutputLogListener.Destroy();
        GDebugOutputLogListener.Destroy();
    }
}
