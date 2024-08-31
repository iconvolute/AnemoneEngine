#include "AnemoneRuntime/Network/Static.hxx"
#include "AnemoneRuntime/Network/Windows/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

namespace Anemone::Network
{
    std::optional<bool> NetworkStatic::HasInternetConnection() const
    {
        VARIANT_BOOL connected = VARIANT_FALSE;

        if (this->NetworkListManager)
        {
            if (FAILED(this->NetworkListManager->get_IsConnectedToInternet(&connected)))
            {
                AE_FATAL("Failed to check internet connection status.");
            }

            return connected == VARIANT_TRUE;
        }

        return std::nullopt;
    }

    UninitializedObject<NetworkStatic> GNetworkStatic{};

    void InitializeStatic()
    {
        // Initialize Networking.
        WSADATA wsaData{};
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        GNetworkStatic.Create();

        // Create network manager.
        if (FAILED(CoCreateInstance(
                CLSID_NetworkListManager,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&GNetworkStatic->NetworkListManager))))
        {
            AE_FATAL("Failed to create INetworkListManager instance.");
        }
    }

    void ShutdownStatic()
    {
        GNetworkStatic.Destroy();

        // Finalize Networking.
        WSACleanup();
    }
}
