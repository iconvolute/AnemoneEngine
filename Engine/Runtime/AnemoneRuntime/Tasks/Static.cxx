#include "AnemoneRuntime/Tasks/Static.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

namespace Anemone::Tasks
{
    UninitializedObject<TaskScheduler> GTaskScheduler{};

    void TasksStatic::Initialize()
    {
        GTaskScheduler.Create(TaskSchedulerOptions{
            System::GetProcessorProperties().LogicalCores - 1,
        });
    }

    void TasksStatic::Finalize()
    {
        GTaskScheduler.Destroy();
    }
}
