#pragma once
#include "AnemoneRuntime/Base/Reference.hxx"

namespace Anemone
{
    class Socket : public Reference<Socket>
    {
    public:
        virtual ~Socket();
    };
}
