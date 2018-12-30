#ifndef FILEINFOBD_H
#define FILEINFOBD_H

#include <QObject>
#include <QVector>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFileSystemWatcher>
#include <QDir>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <functional>

#include "listfiles.h"

class FileInfoBD : public QObject
{
    Q_OBJECT
public:
    explicit FileInfoBD(const std::string& path,
                        FileInfoBD* parentFiBD = nullptr,
                        bool includeHiddenFiles = false,
                        QObject *parent = nullptr);

    // folgender ist ebenfalls default-constructor (da alle args optional sind):
    explicit FileInfoBD(const QFileInfo& fileInfo = QFileInfo(),
                        FileInfoBD* parentFiBD = nullptr,
                        bool includeHiddenFiles = false,
                        QObject *parentm_focus_match_id = nullptr);
    explicit FileInfoBD(const FileInfoBD& fi);
    ~FileInfoBD();

    bool isElapsed() const;
    bool isLoaded() const;

    int getFileCount() const;
    int getFolderCount() const;
    int getContentCount() const;

    void disableSignals(bool disableSignals = true);

//    const QFileInfo getFileAt(int pos) const;
//    int getFileId(const QString fileName) const;

    const QFileInfo& getFileInfo() const;

    void close();

    std::string fileName() const;
    std::string absPath() const;

    void print() const;

    void iterate(std::function<void(FileInfoBD*,
                                    std::string,
                                    std::string,
                                    bool,
                                    bool,
                                    FileInfoBD*)> f);
    void iterate_helper(std::function<void(FileInfoBD*,
                                           std::string,
                                           std::string,
                                           bool,
                                           bool,
                                           FileInfoBD*)> f,
                        bool isCollapsed,
                        FileInfoBD* firstNonCollapsedFold);
signals:
    void contentHasChanged(QString path);

    void addDirectoryToWatcher(QString directory);

    void elapsingFinished();
    void collapsingFinished();
    void showingHiddenFilesFinished();
    void hidingHiddenFilesFinished();
public slots:
    void elapse();
    void elapse_rec();
    void elapse_hlpr();
    void cancelElapsing();

    void collapse();
    void collapse_rec();
    void collapse_hlpr();
    void cancelCollapsing();

    void showHiddenFiles();
    void hideHiddenFiles();
    void showHiddenFiles_rec();
    void hideHiddenFiles_rec();
    void showHiddenFiles_hlpr();
    void hideHiddenFiles_hlpr();
    void cancelHidingHiddenFiles();
private:
    void revalFolderContent();

    void doElapsing();

//    void removeFromParent();
//    bool removeFolder(const QFileInfo& fi);
//    bool removeFolder(const QString& foldName);

    bool isEmpty() const;

    bool m_isElapsed;
    bool m_alrLoaded;

    QFileInfo m_fileInfo;

    std::unordered_map<std::string, QFileInfo> m_files;
    std::unordered_set<FileInfoBD*> m_sub_folds;
    std::unordered_map<std::string, FileInfoBD*> m_sub_fold_names; // only the folder names, not the entire path -> needed for fast lookup
    int m_filesCount;
    int m_subFoldsCount;
    int m_contentCount;

    bool m_showHiddenFiles;

    bool m_disableSignals;

    FileInfoBD* m_parent;

    void resetCancelFlags();
    bool m_cancelled_elapsing;
    bool m_cancelled_collapsing;
    bool m_cancelled_hidingHiddenFiles;
};

#endif // FILEINFOBD_H
