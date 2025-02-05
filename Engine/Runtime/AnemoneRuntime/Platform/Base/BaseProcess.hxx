#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <expected>
#include <optional>

namespace Anemone
{
    class RUNTIME_API BaseProcess
    {
    public:
        BaseProcess() = default;
        BaseProcess(BaseProcess const&) = delete;
        BaseProcess(BaseProcess&&) noexcept = default;
        BaseProcess& operator=(BaseProcess const&) = delete;
        BaseProcess& operator=(BaseProcess&&) noexcept = default;
        ~BaseProcess() = default;

    public:
        static auto Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory)
            -> std::expected<int32_t, ErrorCode>;

        static auto Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            FunctionRef<void(std::string_view)> output,
            FunctionRef<void(std::string_view)> error)
            -> std::expected<int32_t, ErrorCode>;

        static auto Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            std::string& output,
            std::string& error)
            -> std::expected<int32_t, ErrorCode>;
    };
}
