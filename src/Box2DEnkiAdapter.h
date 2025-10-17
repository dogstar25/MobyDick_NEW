// Box2DEnkiAdapter.h
#pragma once
#include <box2d/box2d.h>
#include <enkiTS/TaskScheduler.h>   // vcpkg path for 1.11

class Game;

struct Box2DEnkiAdapter
{
    enki::TaskScheduler* scheduler = nullptr;
    bool                 ownsScheduler = false;

    Box2DEnkiAdapter() = default;
    explicit Box2DEnkiAdapter(enki::TaskScheduler* ext) : scheduler(ext), ownsScheduler(false) {}
    ~Box2DEnkiAdapter();

    // Initialize if needed; numThreads=0 lets enkiTS pick (cores-1)
    void initIfNeeded(int numThreads = 0);

    // Fill b2WorldDef with MT callbacks/params
    void configure(b2WorldDef& def);

    // ---- Box2D task callbacks ----
    static void* EnqueueTask(b2TaskCallback* task, int itemCount, int minRange, void* taskCtx, void* userCtx);
    static void  FinishTask(void* userTask, void* userCtx);

private:
    struct Task : enki::ITaskSet
    {
        b2TaskCallback* cb{};
        void* taskCtx{};

        Task(b2TaskCallback* c, int itemCount, int minRange, void* ctx)
            : cb(c), taskCtx(ctx)
        {
            // enkiTS 1.11 reads these members to partition the work
            m_SetSize = static_cast<uint32_t>(itemCount);
            m_MinRange = static_cast<uint32_t>(std::max(1, minRange));
        }

        void ExecuteRange(enki::TaskSetPartition range, uint32_t threadnum) override
        {
            // Box2D parallel-for: [start, end), workerIndex
            cb(static_cast<int>(range.start),
                static_cast<int>(range.end),
                static_cast<int>(threadnum),
                taskCtx);
        }
    };
};

