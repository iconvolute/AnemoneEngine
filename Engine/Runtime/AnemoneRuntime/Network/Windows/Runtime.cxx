#include "AnemoneRuntime/Network/Runtime.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Network
{
    void InitializeRuntime(RuntimeInitializeContext& context)
    {
        (void)context;

        // Initialize Networking.
        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            AE_PANIC("Failed to initialize networking.");
        }
    }

    void FinalizeRuntime(RuntimeFinalizeContext& context)
    {
        (void)context;

        // Finalize Networking.
        if (WSACleanup() != 0)
        {
            AE_PANIC("Failed to finalize networking.");
        }
    }
}
