#include "AnemoneTasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneTasks/DefaultTaskScheduler.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<DefaultTaskScheduler> gDefaultTaskScheduler{};
    }

    void TaskScheduler::Initialize()
    {
        gDefaultTaskScheduler.Create();
    }

    void TaskScheduler::Finalize()
    {
        gDefaultTaskScheduler.Destroy();
    }

    TaskScheduler& TaskScheduler::Get()
    {
        return *gDefaultTaskScheduler;
    }
}
