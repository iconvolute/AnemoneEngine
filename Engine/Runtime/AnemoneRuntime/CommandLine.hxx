#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <string_view>
#include <span>
#include <vector>
#include <utility>
#include <optional>

namespace Anemone
{
    class RUNTIME_API CommandlineParser
    {
    private:
        std::vector<std::string_view> m_Positional{};
        std::vector<std::pair<std::string_view, std::string_view>> m_Named{};

    public:
        void Parse(int argc, const char* const* argv);

        void Parse(std::string_view commandLine);

        [[nodiscard]] auto GetPositional() const -> std::span<std::string_view const>;

        [[nodiscard]] auto GetNamed() const -> std::span<std::pair<std::string_view, std::string_view> const>;

        [[nodiscard]] auto GetValue(std::string_view name) const -> std::optional<std::string_view>;

        [[nodiscard]] auto GetValues(std::string_view name) const -> std::vector<std::string_view>;

        template <typename CallbackT = void(std::string_view)>
        void EnumerateValues(std::string_view name, CallbackT&& callback)
        {
            for (auto const& [key, value] : this->m_Named)
            {
                if (key == name)
                {
                    std::forward<CallbackT>(callback)(value);
                }
            }
        }

    private:
        static auto ParseToken(std::string_view& commandLine) -> std::string_view;

        void HandleToken(std::string_view token);
    };
}
