#pragma once
#include "AnemoneRuntime/Network/Static.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <netlistmgr.h>
#include <wrl/client.h>

#include <optional>

namespace Anemone::Network
{
    struct NetworkStatic final
    {
        Microsoft::WRL::ComPtr<INetworkListManager> NetworkListManager;

        std::optional<bool> HasInternetConnection() const;
    };

    extern UninitializedObject<NetworkStatic> GNetworkStatic;
}
