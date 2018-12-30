#include "taskfolderelapser.h"

TaskFolderElapser::TaskFolderElapser(FileInfoBD* fiBD,
                                     bool collapse,
                                     bool recursively,
                                     QObject *parent)
    : QueueTask(parent),
      m_fiBD(fiBD),
      m_collapse(collapse),
      m_recursively(recursively)
{
    connectFiBD();
}

void TaskFolderElapser::run()
{
    if(m_collapse)
    {
        if(m_recursively)
        {
            emit elapseFiBD_rec();
        }else{
            emit elapseFiBD();
        }
    }else{
        if(m_recursively)
        {
            emit collapseFiBD_rec();
        }else{
            emit collapseFiBD();
        }
    }
}

void TaskFolderElapser::pause()
{
}

void TaskFolderElapser::continue_()
{
}

void TaskFolderElapser::cancel()
{
    if(m_collapse)
        emit cancelCollapsing();
    else
        emit cancelElapsing();
}

void TaskFolderElapser::elapsingFinished()
{
    disconnectFiBD();
    emit finished(static_cast<QueueTask*>(this));
}

void TaskFolderElapser::collapsingFinished()
{
    disconnectFiBD();
    emit finished(static_cast<QueueTask*>(this));
}

bool TaskFolderElapser::executableInParallel() const
{
    return false;
}

void TaskFolderElapser::connectFiBD()
{
    connect(this, &TaskFolderElapser::elapseFiBD,       m_fiBD, &FileInfoBD::elapse);
    connect(this, &TaskFolderElapser::elapseFiBD_rec,   m_fiBD, &FileInfoBD::elapse_rec);
    connect(this, &TaskFolderElapser::collapseFiBD,     m_fiBD, &FileInfoBD::collapse);
    connect(this, &TaskFolderElapser::collapseFiBD_rec, m_fiBD, &FileInfoBD::collapse_rec);
    connect(this, &TaskFolderElapser::cancelElapsing,   m_fiBD, &FileInfoBD::cancelElapsing);
    connect(this, &TaskFolderElapser::cancelCollapsing, m_fiBD, &FileInfoBD::cancelCollapsing);

    connect(m_fiBD, &FileInfoBD::elapsingFinished,   this, &TaskFolderElapser::elapsingFinished);
    connect(m_fiBD, &FileInfoBD::collapsingFinished, this, &TaskFolderElapser::collapsingFinished);
}

void TaskFolderElapser::disconnectFiBD()
{
    disconnect(this, &TaskFolderElapser::elapseFiBD,       m_fiBD, &FileInfoBD::elapse);
    disconnect(this, &TaskFolderElapser::elapseFiBD_rec,   m_fiBD, &FileInfoBD::elapse_rec);
    disconnect(this, &TaskFolderElapser::collapseFiBD,     m_fiBD, &FileInfoBD::collapse);
    disconnect(this, &TaskFolderElapser::collapseFiBD_rec, m_fiBD, &FileInfoBD::collapse_rec);
    disconnect(this, &TaskFolderElapser::cancelElapsing,   m_fiBD, &FileInfoBD::cancelElapsing);
    disconnect(this, &TaskFolderElapser::cancelCollapsing, m_fiBD, &FileInfoBD::cancelCollapsing);

    disconnect(m_fiBD, &FileInfoBD::elapsingFinished,   this, &TaskFolderElapser::elapsingFinished);
    disconnect(m_fiBD, &FileInfoBD::collapsingFinished, this, &TaskFolderElapser::collapsingFinished);
}
