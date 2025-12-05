#include "AnemoneEntryPoint/EntryPoint.hxx"

#include <catch_amalgamated.hpp>

void TestRuntime_ModuleInitialize() { }
void TestRuntime_ModuleFinalize() { }

int AnemoneMain(int argc, char* argv[])
{
    return Catch::Session().run(argc, argv);
}
