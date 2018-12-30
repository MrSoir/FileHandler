#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <functional>
#include <utility>

#include "fileinfobd.h"
#include "filesearcher.h"
#include "fileselector.h"
#include "fibdviewer.h"
#include "filequeue.h"
#include "taskfolderelapser.h"
#include "filewatcher.h"

class FileViewer : public QObject
{
    Q_OBJECT
signals:
    virtual void cancel() = 0;
    virtual void pause() = 0;
public slots:
    virtual void repaint(std::vector<FiBDViewer> fileToRepaint, int firstIdToRender = -1, int lastIdToRender = -1) = 0;
    virtual void startWaitingLoop() = 0;
    virtual void exitWaitingLoop() = 0;
};

class FiBD {
public:
    explicit FiBD(FileInfoBD* fiBD, bool isFolder)
        : fiBD(fiBD),
          isFolder(isFolder)
    {}

    FileInfoBD* fiBD;
    bool isFolder;
};

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(std::string root_path);
    ~FileManager();

    void test();
signals:
    void revalidateViewer(std::vector<FiBDViewer> entries);

    // searching:
    void search(std::string key_word);
    void clearSearch();
    void hideSearch();
    void showSearch();
    void nextSearchResult();
    void prevSearchResult();

    // teilt dem Viewer mit, in die warteschleife zu verfallen bzw. wieder auszubrechen:
    void setViewerIntoWaitingLoop();
    void exitViewerWaitingLoop();
    void cancelFileQueueTask();
    void pauseFileQueueTask();
    void continueFileQueueTask();
    void startWaitingLoop();
    void exitWaitingLoop();
public slots:
    void setRoot(const std::string& rootPath);

    void selectionChanged();
    void searchResultsChanged();

    void directoryChanged(std::string path);
private:
    void deleteRoot();

    void createEntries();

//    --------------------------------

    FileInfoBD* root_folder;

    std::unordered_map<std::string, FileInfoBD*> m_folders; // path to FileInfoBD

    std::unordered_map<std::string, std::string> m_entry_to_firstElapsedFolder; // FileSearcher: hidden folders: path -> path of first non-hidden/elapsed folder

    std::unordered_map<std::string, int> m_entries_order; // FileSearcher: path to order/id in list
    std::unordered_map<int, std::string> m_order_entries; // FileSelector: order/id in list to path
    std::unordered_map<std::string, int> m_entries_order_colpsd; // FileSearcher: path to order/id in list
    std::unordered_map<int, std::string> m_order_entries_colpsd; // FileSelector: order/id in list to path

    std::unordered_map<std::string, FiBD> m_entries; // FileNamanger: path to FiBD struct {FileInfoBD | isFolder}
    std::unordered_map<std::string, FiBD> m_entries_colpsd; // FileNamanger: path to FiBD struct {FileInfoBD | isFolder}

    std::unordered_map<std::string, std::string> m_fileNames; // FileSearcher: path to fileName
    std::unordered_map<std::string, std::string> m_fileNames_colpsd; // FileSearcher: path to fileName

    FileSearcher* m_searcher;
    FileSelector* m_selector;
    FileQueue* m_tasks_queue;

    FileWatcher* m_watcher;

    FileViewer* m_viewer;

    int m_frstDispFile;
    int m_lastDispFile;
};

#endif // FILEMANAGER_H
