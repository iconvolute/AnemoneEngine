#include "AnemoneRuntime/Network/Static.hxx"
#include "AnemoneRuntime/Network/Windows/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

namespace Anemone::Network
{
    std::optional<bool> HasInternetConnection()
    {
        auto& native = GNetworkStatic.Get();

        VARIANT_BOOL connected = VARIANT_FALSE;

        if (native.NetworkListManager)
        {
            if (FAILED(native.NetworkListManager->get_IsConnectedToInternet(&connected)))
            {
                AE_PANIC("Failed to check internet connection status.");
            }

            return connected == VARIANT_TRUE;
        }

        return std::nullopt;
    }
}
