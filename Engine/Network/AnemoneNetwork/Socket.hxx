#pragma once
#include "AnemoneRuntime/Base/Reference.hxx"

namespace Anemone
{
    class Socket : public ReferenceCounted<Socket>
    {
    public:
        virtual ~Socket();
    };
}
