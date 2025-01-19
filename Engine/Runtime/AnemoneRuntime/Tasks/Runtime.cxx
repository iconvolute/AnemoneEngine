#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

namespace Anemone::Tasks
{
#if false
    UninitializedObject<TaskScheduler> GTaskScheduler{};

    void InitializeRuntime(RuntimeInitializeContext& context)
    {
        (void)context;

        GTaskScheduler.Create(TaskSchedulerOptions{
            System::GetProcessorProperties().LogicalCores - 1,
        });
    }

    void FinalizeRuntime(RuntimeFinalizeContext& context)
    {
        (void)context;

        GTaskScheduler.Destroy();
    }
#endif
}
