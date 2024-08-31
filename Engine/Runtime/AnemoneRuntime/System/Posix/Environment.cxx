#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/System/Path.hxx"
#include "AnemoneRuntime/System/Static.hxx"

namespace Anemone::System
{
    bool GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        // TODO: promote win32_string_buffer and use it here instead of this `std::string`
#if ANEMONE_PLATFORM_ANDROID
        char const* variable = getenv(std::string{name}.c_str());
#else
        char const* variable = secure_getenv(std::string{name}.c_str());
#endif

        if (variable != nullptr)
        {
            result.assign(variable);
        }

        return variable != nullptr;
    }

    bool SetEnvironmentVariable(std::string_view name, std::string_view value)
    {
        return setenv(std::string{name}.c_str(), std::string{value}.c_str(), 1) == 0;
    }

    bool RemoveEnvironmentVariable(std::string_view name)
    {
        return unsetenv(std::string{name}.c_str()) == 0;
    }
}

namespace Anemone::System
{
    bool GetClipboardContent(std::string& value)
    {
        (void)value;
        return false;
    }

    bool SetClipboardContent(std::string_view value)
    {
        (void)value;
        return false;
    }
}

namespace Anemone::System
{
    void GetDisplayMetrics(DisplayMetrics& displayMetrics)
    {
        displayMetrics.Displays.clear();
    }
}

namespace Anemone::System
{
    ColorRef GetScreenPixel(Point<int32_t> position, float gamma)
    {
        (void)position;
        (void)gamma;
        return ColorRef{};
    }
}

namespace Anemone::System
{
    ProcessorProperties const& GetProcessorProperties()
    {
        return GProcessorProperties.Get();
    }
}

namespace Anemone::System
{
    Environment const& GetEnvironment()
    {
        return GEnvironment.Get();
    }
}
