#pragma once
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Private
{
    struct CommandLineStatics final
    {
        int ArgC = 0;
        char** ArgV = nullptr;

        CommandLineStatics(int argc, char* argv[])
            : ArgC{argc}
            , ArgV{argv}
        {
            AE_ASSERT(ArgC > 0);
            AE_ASSERT(ArgV != nullptr);
        }

        ~CommandLineStatics() = default;
    };

    extern UninitializedObject<CommandLineStatics> GCommandLineStatics;
}
