// Box2DEnkiAdapter.cpp
#include "Box2DEnkiAdapter.h"
#include "game.h"

Box2DEnkiAdapter::~Box2DEnkiAdapter()
{
    if (ownsScheduler && scheduler)
    {
        scheduler->ShutdownNow();
        delete scheduler;
        scheduler = nullptr;
    }
}

void Box2DEnkiAdapter::initIfNeeded(int numThreads)
{
    if (scheduler) return;

    scheduler = new enki::TaskScheduler();
    ownsScheduler = true;

    enki::TaskSchedulerConfig cfg;
    if (numThreads > 0) cfg.numTaskThreadsToCreate = numThreads;
    scheduler->Initialize(cfg);
}

void Box2DEnkiAdapter::configure(b2WorldDef& def)
{
    initIfNeeded(); // ensure scheduler exists

    // Tell Box2D how many workers it can use (not counting the calling thread)
    def.workerCount = static_cast<int>(scheduler->GetNumTaskThreads());
    def.userTaskContext = scheduler;

    def.enqueueTask = &Box2DEnkiAdapter::EnqueueTask;
    def.finishTask = &Box2DEnkiAdapter::FinishTask;
}

void* Box2DEnkiAdapter::EnqueueTask(b2TaskCallback* task,
    int itemCount,
    int minRange,
    void* taskCtx,
    void* userCtx)
{
    auto* sched = static_cast<enki::TaskScheduler*>(userCtx);

    // If no range, just run synchronously and return nullptr.
    if (itemCount <= 0)
    {
        task(0, 0, /*worker*/0, taskCtx);
        return nullptr;
    }

    // Create a task that embeds range + grain size
    auto* t = new Task(task, itemCount, minRange, taskCtx);

    // enkiTS 1.11: just enqueue the task; it will partition using m_SetSize/m_MinRange
    sched->AddTaskSetToPipe(t);

    // Return handle for FinishTask
    return t;
}

void Box2DEnkiAdapter::FinishTask(void* userTask, void* userCtx)
{
    if (!userTask) return; // nothing to wait on (ran synchronously)

    auto* sched = static_cast<enki::TaskScheduler*>(userCtx);
    auto* t = static_cast<Task*>(userTask);

    // Wait for completion (this thread may help execute)
    sched->WaitforTaskSet(t);

    delete t;
}

