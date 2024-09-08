#pragma once
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <netlistmgr.h>
#include <wrl/client.h>

namespace Anemone::Network
{
    struct NetworkStatic final
    {
        Microsoft::WRL::ComPtr<INetworkListManager> NetworkListManager;
    };

    extern UninitializedObject<NetworkStatic> GNetworkStatic;
}
