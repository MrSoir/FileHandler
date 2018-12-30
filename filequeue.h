#ifndef FILEQUEUE_H
#define FILEQUEUE_H

#include <QObject>
#include <queue>
#include <functional>

// GAAANZ WICHTIGER HINWEIS:
// FileQueue geht folgendermassen vor:
//      wenn es einen QueueTask cancelled, dann wird erwartet, dass der QueueTask
//      dennoch finished, d.h. das signal finished(QueueTask*) sendet.
//      -> nur eben etwas frueher, da durch das cancellen, die routine QueueTask::run()
//      frueher zum abschluss kommen sollte!
// das macht das ganze etwas einfacher zu programmieren. somit muss ausschliesslich
// das signal QueueTask::finished(QueueTask*) => FileQueue::taskFinished(QueueTask*) abgefangen werden und nicht auch noch
// das signal QueueTask::cancelled(QueueTask*) => FileQueue::taskCancelled(QueueTask*)
// semantisch bleibt es aber natuerlich fragwuerdig, nach einem canceln noch zu finishen...

class QueueTask : public QObject
{
    Q_OBJECT
signals:
    virtual void finished(QueueTask*);
public slots:
    virtual void run() = 0;
    virtual void pause() = 0;
    virtual void continue_() = 0;
    virtual void cancel() = 0;
public:
    virtual bool executableInParallel() const = 0; // FileQueue muss QueueTask nicht auf Queue-Stack legen sondern kann ihn direkt starten, selbst wenn ein anderer Task laeuft. Beispiel: kopieren von dateien kann parallel ablaufen, da die aufgabe unabhaengig vom FileExplorer ist. Hingegen muss FileInfoBD.elapseAll() zwingend auf den Queue gepackt werden.

    // default constructor & copy constructor & destructor sind gefordert, wenn QueueTask zwecks signal-slot angemeldet/registriert werden muss:
    QueueTask(QObject* parent = nullptr) : QObject(parent)
    {}
    QueueTask(const QueueTask& task) : QObject(nullptr)
    {
        Q_UNUSED(task);
    }
    ~QueueTask(){}
};

class FileQueue : public QObject
{
    Q_OBJECT
public:
    explicit FileQueue();
    ~FileQueue();
signals:
    void paused();
    void continued();
    void cancelled();

    // teilt dem FileManager mit, in die warteschleife zu gehen und diese wieder zu beenden:
    void blockUntilTaskHasFinish();
    void blockingTaskHasFinished();
public slots:
    void addTask(QueueTask* task);

    void pause();
    void continue_();
    void cancel();

    void taskFinished(QueueTask* task);
private:
    void processTasks();

    void connectTask(QueueTask* task);
    void disconnectTask(QueueTask* task);

    std::queue<QueueTask*> m_tasks;

    bool m_taskIsRunning;
};

#endif // FILEQUEUE_H
