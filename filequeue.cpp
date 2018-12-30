#include "filequeue.h"

FileQueue::FileQueue()
    : QObject(nullptr),
      m_tasks(std::queue<QueueTask*>()),
      m_taskIsRunning(false)
{
}

FileQueue::~FileQueue()
{
    emit cancelled();
    while(auto task = m_tasks.front())
    {
        delete task;
        m_tasks.pop();
    }
}

void FileQueue::addTask(QueueTask* task)
{
    m_tasks.push(task);
    if( task->executableInParallel() )
    {
        task->run();
    }else if( !m_taskIsRunning )
    {
        processTasks();
    }
}


void FileQueue::pause()
{
    emit paused();
}
void FileQueue::continue_()
{
    emit continued();
}
void FileQueue::cancel()
{
    emit cancelled();
}


void FileQueue::taskFinished(QueueTask* task)
{
    disconnectTask(task);
//    delete task;
    task->deleteLater();

    if( m_tasks.size() > 0 )
    {
        processTasks();
    }else{
        m_taskIsRunning = false;
        emit blockingTaskHasFinished();
    }
}


void FileQueue::processTasks()
{
    auto task = m_tasks.front();
    m_tasks.pop();
    if(task)
    {
        m_taskIsRunning = true;
        emit blockUntilTaskHasFinish();

        connectTask(task);

        task->run();
    }
}

void FileQueue::connectTask(QueueTask *task)
{
    connect(this, &FileQueue::cancelled,   task, &QueueTask::cancel);
    connect(this, &FileQueue::paused,      task, &QueueTask::pause);
    connect(this, &FileQueue::continued,   task, &QueueTask::continue_);

    connect(task, &QueueTask::finished, this, &FileQueue::taskFinished);
}
void FileQueue::disconnectTask(QueueTask *task)
{
    disconnect(this, &FileQueue::cancelled,   task, &QueueTask::cancel);
    disconnect(this, &FileQueue::paused,      task, &QueueTask::pause);
    disconnect(this, &FileQueue::continued,   task, &QueueTask::continue_);

    disconnect(task, &QueueTask::finished, this, &FileQueue::taskFinished);
}
