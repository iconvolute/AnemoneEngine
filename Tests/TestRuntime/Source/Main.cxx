#include "AnemoneRuntime/Platform/EntryPoint.hxx"

#include <catch_amalgamated.hpp>

int AnemoneMain(int argc, char* argv[])
{
    // Anemone::Platform::Internal::Initialize(argc, argv);

    int const result = Catch::Session().run(argc, argv);

    // Anemone::Platform::Internal::Finalize();

    return result;
}
