#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Runtime.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

int main(int argc, char* argv[])
{
    Anemone::RuntimeInitializer runtime{argc, argv};

    return Catch::Session().run(argc, argv);
}
