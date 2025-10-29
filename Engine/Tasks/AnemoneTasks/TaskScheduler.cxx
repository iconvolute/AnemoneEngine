#include "AnemoneTasks/TaskScheduler.hxx"
#include "AnemoneBase/UninitializedObject.hxx"
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
