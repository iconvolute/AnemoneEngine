#include "AnemoneEntryPoint/EntryPoint.hxx"

#include <catch_amalgamated.hpp>

void TestNumerics_ModuleInitialize() { }
void TestNumerics_ModuleFinalize() { }

int AnemoneMain(int argc, char* argv[])
{
    return Catch::Session().run(argc, argv);
}
