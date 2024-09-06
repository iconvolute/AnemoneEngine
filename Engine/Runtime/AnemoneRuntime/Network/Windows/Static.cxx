#include "AnemoneRuntime/Network/Static.hxx"
#include "AnemoneRuntime/Network/Windows/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

namespace Anemone::Network
{
    UninitializedObject<WindowsNetworkStatic> GNetworkStatic{};

    void WindowsNetworkStatic::Initialize()
    {
        GenericNetworkStatic::Initialize();

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

    void WindowsNetworkStatic::Finalize()
    {
        // Finalize Networking.
        WSACleanup();

        GNetworkStatic.Destroy();

        GenericNetworkStatic::Finalize();
    }

    std::optional<bool> WindowsNetworkStatic::HasInternetConnection() const
    {
        VARIANT_BOOL connected = VARIANT_FALSE;

        if (this->NetworkListManager)
        {
            if (FAILED(this->NetworkListManager->get_IsConnectedToInternet(&connected)))
            {
                AE_PANIC("Failed to check internet connection status.");
            }

            return connected == VARIANT_TRUE;
        }

        return std::nullopt;
    }
}
