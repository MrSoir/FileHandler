#include "filemanager.h"

FileManager::FileManager(std::string root_path)
    : QObject(nullptr),
      root_folder(nullptr),
      m_searcher(new FileSearcher(this)),
      m_selector(new FileSelector(this)),
      m_tasks_queue(new FileQueue()),
      m_watcher(nullptr),
      m_viewer(nullptr), // m_viewer muss dann vom eigentlichen QWidget-FileViewer hier initialisiert werden!!!
      m_frstDispFile(0),
      m_lastDispFile(0)
{
    QObject::connect(this, &FileManager::search, m_searcher, &FileSearcher::search);

    QObject::connect(this, &FileManager::cancelFileQueueTask,   m_tasks_queue, &FileQueue::cancel);
    QObject::connect(this, &FileManager::pauseFileQueueTask,    m_tasks_queue, &FileQueue::pause);
    QObject::connect(this, &FileManager::continueFileQueueTask, m_tasks_queue, &FileQueue::continue_);
    QObject::connect(m_tasks_queue, &FileQueue::blockUntilTaskHasFinish, this, &FileManager::startWaitingLoop);
    QObject::connect(m_tasks_queue, &FileQueue::blockingTaskHasFinished, this, &FileManager::exitWaitingLoop);

    if(m_viewer) // if, da m_viewer zu test-zwecken noch auf nullptr gesetzt ist. im eigentlichen programm dann eig nicht mehr notwendig darauf zu testen im konstruktor!
    {
        QObject::connect(m_viewer, &FileViewer::cancel, this, &FileManager::cancelFileQueueTask);
        QObject::connect(m_viewer, &FileViewer::pause,  this, &FileManager::pauseFileQueueTask);
        QObject::connect(this, &FileManager::startWaitingLoop, m_viewer, &FileViewer::startWaitingLoop);
        QObject::connect(this, &FileManager::exitWaitingLoop,  m_viewer, &FileViewer::exitWaitingLoop);
    }

    setRoot(root_path);
}

FileManager::~FileManager()
{
    deleteRoot();
    delete m_searcher;
    delete m_selector;
    m_tasks_queue->deleteLater();
    delete m_watcher;
    m_viewer->deleteLater();
//    m_searcher->deleteLater();
//    m_searcher->deleteLater();
//    m_tasks_queue->deleteLater();
//    m_watcher->deleteLater();
}

void FileManager::test()
{
    selectionChanged();
}

void FileManager::setRoot(const std::string& rootPath)
{
    if(root_folder)
        deleteRoot();

    if(m_watcher)
        m_watcher->deleteLater();

    m_watcher = new FileWatcher(this);
    connect(m_watcher, &FileWatcher::directoryChanged, this, &FileManager::directoryChanged);


    if(root_folder && root_folder->absPath() != rootPath)
    {
        root_folder = new FileInfoBD(rootPath);
        createEntries();
    }
}

void FileManager::selectionChanged()
{
    std::vector<FiBDViewer> entries;
    for(int i=m_frstDispFile; i <= m_lastDispFile; ++i)
    {
        FiBDViewer fiView;
        entries.push_back(fiView);
    }
    emit revalidateViewer(entries);
}

void FileManager::searchResultsChanged()
{
}

void FileManager::directoryChanged(std::string path)
{
    auto fiBD = m_folders[path];
    if(fiBD)
    {
        fiBD->revalidate();
    }
}


void FileManager::deleteRoot()
{
    if(root_folder)
    {
        root_folder->close();
        delete root_folder;
    }
}

void FileManager::createEntries()
{
    int* order = new int(0);
    int* order_clpsd = new int(0);

    root_folder->iterate(
             [=](FileInfoBD* fiBD,
                 std::string path,
                 std::string fileName,
                 bool isFolder,
                 bool isCollapsed,
                 FileInfoBD* firstNonCollapsedFold)
             {
                 if(isFolder)
                 {
                     m_folders[path] = fiBD;
                     m_watcher->addPath(path);
                 }

                 m_entry_to_firstElapsedFolder[path] = firstNonCollapsedFold->absPath();
                 m_entries.emplace(std::make_pair(path, FiBD(fiBD, isFolder)));
                 m_entries_order[path] = *order;
                 m_order_entries[*order] = path;
                 m_fileNames[path] = fileName;
                 ++(*order);

                 if(!isCollapsed)
                 {
                     m_entries_colpsd.emplace(std::make_pair(path, FiBD(fiBD, isFolder)));
                     m_entries_order_colpsd[path] = *order;
                     m_order_entries_colpsd[*order_clpsd] = path;
                     m_fileNames_colpsd[path] = fileName;
                     ++(*order_clpsd);
                 }
             }
    );

    delete order;
    delete order_clpsd;
}
