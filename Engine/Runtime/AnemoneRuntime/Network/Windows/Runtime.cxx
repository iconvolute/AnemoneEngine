#include "AnemoneRuntime/Network/Runtime.hxx"
#include "AnemoneRuntime/Network/Windows/Static.hxx"

namespace Anemone::Network
{
    UninitializedObject<NetworkStatic> GNetworkStatic{};

    void InitializeRuntime(RuntimeInitializeContext& context)
    {
        (void)context;

        GNetworkStatic.Create();

        // Initialize Networking.
        WSADATA wsaData{};
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        // Create network manager.
        if (FAILED(CoCreateInstance(
                CLSID_NetworkListManager,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&GNetworkStatic->NetworkListManager))))
        {
            AE_PANIC("Failed to create INetworkListManager instance.");
        }
    }

    void FinalizeRuntime(RuntimeFinalizeContext& context)
    {
        (void)context;

        // Finalize Networking.
        WSACleanup();

        GNetworkStatic.Destroy();
    }
}
