#include "AnemoneRuntime/Platform/Platform.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

int main(int argc, char* argv[])
{
    // Anemone::Platform::Internal::Initialize(argc, argv);

    int const result = Catch::Session().run(argc, argv);

    // Anemone::Platform::Internal::Finalize();

    return result;
}
