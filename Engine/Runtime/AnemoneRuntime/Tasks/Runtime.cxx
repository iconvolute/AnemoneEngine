#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"

namespace Anemone::Tasks
{
    UninitializedObject<TaskScheduler> GTaskScheduler{};

#if false
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
