#include "AnemoneRuntime/Runtime/EntryPoint.hxx"
#include "AnemoneTasks/Module.hxx"

#include <catch_amalgamated.hpp>

int AnemoneMain(int argc, char* argv[])
{
    Anemone::ModuleInitializer<Anemone::Module_Tasks> moduleTasks{};
    return Catch::Session().run(argc, argv);
}
