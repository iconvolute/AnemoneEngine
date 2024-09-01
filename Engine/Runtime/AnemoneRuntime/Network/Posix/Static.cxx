#include "AnemoneRuntime/Network/Static.hxx"

namespace Anemone::Network
{
    void PosixNetworkStatic::Initialize()
    {
        GenericNetworkStatic::Initialize();
    }

    void PosixNetworkStatic::Finalize()
    {
        GenericNetworkStatic::Finalize();
    }
}
