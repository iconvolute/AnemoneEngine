#include "AnemoneRuntime/Runtime/EntryPoint.hxx"

#include <catch_amalgamated.hpp>

int AnemoneMain(int argc, char* argv[])
{
    return Catch::Session().run(argc, argv);
}
