#include "AnemoneTasks/Module.hxx"
#include "AnemoneTasks/TaskScheduler.hxx"

namespace Anemone
{
    void Module_Tasks::Initialize()
    {
        TaskScheduler::Initialize();
    }

    void Module_Tasks::Finalize()
    {
        TaskScheduler::Finalize();
    }
}
