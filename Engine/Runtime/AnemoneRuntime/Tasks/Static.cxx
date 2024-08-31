#include "AnemoneRuntime/Tasks/Static.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

namespace Anemone::Tasks
{
    UninitializedObject<TaskScheduler> GTaskScheduler{};

    void InitializeStatic()
    {
        GTaskScheduler.Create(TaskSchedulerOptions{
            System::GetProcessorProperties().LogicalCores - 1,
        });
    }

    void ShutdownStatic()
    {
        GTaskScheduler.Destroy();
    }
}
