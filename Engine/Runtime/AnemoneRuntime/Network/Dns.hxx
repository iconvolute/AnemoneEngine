#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Network/IpEndPoint.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <vector>
#include <string_view>
#include <expected>

namespace Anemone::Network
{
    RUNTIME_API std::expected<void, ErrorCode> Resolve(
        std::vector<IpEndPoint>& result,
        std::string_view name,
        std::optional<std::string_view> const& service = {});

    RUNTIME_API std::expected<void, ErrorCode> Resolve(
        std::vector<std::string>& result,
        IpEndPoint const& endpoint);
}
