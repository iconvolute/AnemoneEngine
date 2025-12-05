#include "AnemoneTasks/TaskScheduler.hxx"

TASKS_API void AnemoneTasks_ModuleInitialize()
{
    Anemone::TaskScheduler::Initialize();
}

TASKS_API void AnemoneTasks_ModuleFinalize()
{
    Anemone::TaskScheduler::Finalize();
}
