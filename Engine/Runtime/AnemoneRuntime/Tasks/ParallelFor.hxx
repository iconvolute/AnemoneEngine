#pragma once
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Tasks/Awaiter.hxx"
#include "AnemoneRuntime/Threading/ThisThread.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"

namespace Anemone::Tasks
{
    template <typename TCallback = void(size_t, size_t), typename TFinalize = void(size_t)>
    void ParallelFor(
        size_t count,
        size_t batch,
        TCallback&& callback,
        TFinalize&& finalize,
        size_t threads = 0,
        TaskPriority priority = TaskPriority::Inherited)
    {
        AE_ASSERT(batch != 0);

        if (count == 0)
        {
            // Make sure to call the finalize callback when the count is zero.
            finalize(count);
            return;
        }

        class ParallelForBarrier final : public Task
        {
        public:
            size_t m_Count;
            size_t m_Batch;
            std::atomic<size_t> m_Index{};
            TCallback m_Callback;
            TFinalize m_Finalize;

        public:
            ParallelForBarrier(
                size_t count,
                size_t batch,
                TCallback&& callback,
                TFinalize&& finalize)
                : m_Count{count}
                , m_Batch{batch}
                , m_Callback{std::forward<TCallback>(callback)}
                , m_Finalize{std::forward<TFinalize>(finalize)}
            {
            }

            bool Partition(size_t& first, size_t& last)
            {
                size_t const current = this->m_Index.fetch_add(this->m_Batch, std::memory_order::acquire);

                if (current < this->m_Count)
                {
                    first = current;
                    last = std::min<size_t>(current + this->m_Batch, this->m_Count);
                    return true;
                }

                return false;
            }

        protected:
            void OnExecute() override
            {
                this->m_Finalize(this->m_Count);
            }
        };

        class ParallelForTask final : public Task
        {
        private:
            ParallelForBarrier* m_Parent;

        public:
            explicit ParallelForTask(ParallelForBarrier* parent)
                : m_Parent{parent}
            {
            }

        protected:
            void OnExecute() override
            {
                size_t first;
                size_t last;

                while (this->m_Parent->Partition(first, last))
                {
                    this->m_Parent->m_Callback(first, last);
                }
            }
        };

        // Number of worker threads in pool.
        auto workersCount = std::max<size_t>(1, GTaskScheduler->GetWorkerCount());

        if (threads == 0)
        {
            // Caller did not specify the number of threads.
            threads = workersCount;
        }
        else
        {
            // Adjust the number of threads to the number of workers.
            threads = std::min<size_t>(threads, workersCount);
        }

        // Estimated number of slices.
        size_t const slices = (count / batch) + 1;

        // Adjust the number of threads to the number of slices.
        threads = std::min<size_t>(threads, slices);

        AwaiterHandle joinCounter = new Awaiter{};
        AwaiterHandle forkCounter = new Awaiter{};

        TaskHandle barrier{
            new ParallelForBarrier{
                count,
                batch,
                std::forward<TCallback>(callback),
                std::forward<TFinalize>(finalize),
            },
        };

        for (size_t i = 0; i < threads; ++i)
        {
            TaskHandle task{
                new ParallelForTask{
                    static_cast<ParallelForBarrier*>(barrier.Get()),
                },
            };

            GTaskScheduler->Dispatch(*task, forkCounter, joinCounter, priority);
        }

        AwaiterHandle waitCounter = new Awaiter{};
        GTaskScheduler->Dispatch(*barrier, joinCounter, waitCounter, priority);

        GTaskScheduler->Wait(waitCounter);
    }
}
