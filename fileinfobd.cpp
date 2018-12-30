#include "fileinfobd.h"


//----------------------------------------------------------------


FileInfoBD::FileInfoBD(const std::string& path,
                       FileInfoBD* parentFiBD,
                       bool showHiddenFiles,
                       QObject *parent)
    : QObject(parent),      

      m_isElapsed(false),
      m_alrLoaded(false),

      m_fileInfo(QFileInfo(QString(path.c_str()))),

      m_files(std::unordered_map<std::string, QFileInfo>()),
      m_sub_folds(std::unordered_set<FileInfoBD*>()),
      m_sub_fold_names(std::unordered_map<std::string, FileInfoBD*>()),

      m_filesCount(0),
      m_subFoldsCount(0),
      m_contentCount(0),

      m_showHiddenFiles(showHiddenFiles),
      m_disableSignals(false),

      m_parent(parentFiBD),

      m_cancelled_elapsing(false),
      m_cancelled_collapsing(false),
      m_cancelled_hidingHiddenFiles(false)
{
}
FileInfoBD::FileInfoBD(const QFileInfo& fileInfo,
                    FileInfoBD* parentFiBD,
                    bool showHiddenFiles,
                    QObject *parent)
    : QObject(parent),

      m_isElapsed(false),
      m_alrLoaded(false),

      m_fileInfo(fileInfo),

      m_files(std::unordered_map<std::string, QFileInfo>()),
      m_sub_folds(std::unordered_set<FileInfoBD*>()),
      m_sub_fold_names(std::unordered_map<std::string, FileInfoBD*>()),

      m_filesCount(0),
      m_subFoldsCount(0),
      m_contentCount(0),

      m_showHiddenFiles(showHiddenFiles),
      m_disableSignals(false),

      m_parent(parentFiBD),

      m_cancelled_elapsing(false),
      m_cancelled_collapsing(false),
      m_cancelled_hidingHiddenFiles(false)
{
}

FileInfoBD::FileInfoBD(const FileInfoBD &fi)
    : QObject(fi.parent()),

      m_isElapsed(fi.m_isElapsed),
      m_alrLoaded(fi.m_alrLoaded),

      m_fileInfo(fi.m_fileInfo),

      m_files(fi.m_files),
      m_sub_folds(fi.m_sub_folds),
      m_sub_fold_names(fi.m_sub_fold_names),

      m_filesCount(fi.m_filesCount),
      m_subFoldsCount(fi.m_subFoldsCount),
      m_contentCount(fi.m_contentCount),

      m_showHiddenFiles(fi.m_showHiddenFiles),
      m_disableSignals(fi.m_disableSignals),

      m_parent(fi.m_parent),

      m_cancelled_elapsing(false),
      m_cancelled_collapsing(false),
      m_cancelled_hidingHiddenFiles(false)
{
}

FileInfoBD::~FileInfoBD()
{
    m_files.clear();
    m_sub_fold_names.clear();

    m_filesCount = 0;
    m_subFoldsCount = 0;
    m_contentCount = 0;

    m_parent->m_sub_folds.erase(this);

    for(auto it = m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        delete (*it);
    }
}
//-----------------------------------------------------

bool FileInfoBD::isElapsed() const
{
    return m_isElapsed;
}
bool FileInfoBD::isLoaded() const
{
    return m_alrLoaded;
}

int FileInfoBD::getFileCount() const
{
    return m_filesCount;
}
int FileInfoBD::getFolderCount() const
{
    return m_subFoldsCount;
}
int FileInfoBD::getContentCount() const
{
    return m_contentCount;
}

void FileInfoBD::disableSignals(bool disableSignals)
{
    m_disableSignals = disableSignals;
}


void FileInfoBD::close()
{
    m_disableSignals = true;
    for(auto it = m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->close();
        delete (*it);
    }
    m_sub_folds.clear();
    m_files.clear();
    m_sub_fold_names.clear();
//    removeFromParent(m_fileInfo);
//    m_parent.reset();
}

std::string FileInfoBD::fileName() const
{
    return m_fileInfo.fileName().toStdString();
}
std::string FileInfoBD::absPath() const
{
    return m_fileInfo.absolutePath().toStdString();
}

void FileInfoBD::print() const
{
    qDebug() << m_fileInfo.fileName();
    int i=0;
    for(auto it = m_files.begin(); it != m_files.end(); ++it, ++i)
    {
        qDebug() << "file[" << i << "]: " << QString::fromStdString(it->first);
    }
    for(auto it = m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->print();
    }
}

void FileInfoBD::iterate(std::function<void(FileInfoBD*,
                                            std::string,
                                            std::string,
                                            bool,
                                            bool,
                                            FileInfoBD*)> f)
{
    bool isCollapsed = m_parent ? m_parent->m_isElapsed : false;
    FileInfoBD* fncf = this; // fncf = firstNonCollapsedFold
    while(fncf)
    {
        if(fncf->m_parent)
        {
            if(fncf->m_parent->m_isElapsed)
            {
                fncf = fncf->m_parent;
            }else{
                break;
            }
        }else{
            break;
        }
    }
    iterate_helper(f, isCollapsed, fncf);
}
void FileInfoBD::iterate_helper(std::function<void(FileInfoBD*,
                                                   std::string,
                                                   std::string,
                                                   bool,
                                                   bool,
                                                   FileInfoBD* firstNonCollapsedFold)> f,
                                bool isCollapsed,
                                FileInfoBD* firstNonCollapsedFold)
{
    FileInfoBD* fncf = m_isElapsed ? this : firstNonCollapsedFold;

    f(this, absPath(), fileName(), true, isCollapsed, fncf);

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->iterate_helper(f, isCollapsed || !m_isElapsed, fncf);
    }
    for(auto it: m_files)
    {
        std::string filePath = it.first;
        std::string fileName = it.second.fileName().toStdString();

        f(this, filePath, fileName, false, isCollapsed, fncf);
    }
}
//-----------------------------------------------------

// elapsing-functions:
void FileInfoBD::elapse()
{
    resetCancelFlags();

    elapse_hlpr();
    emit elapsingFinished();
}
void FileInfoBD::elapse_rec()
{
    resetCancelFlags();

    elapse_hlpr();

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_elapsing)
            break;

        (*it)->elapse_rec();
    }
    emit elapsingFinished();
}
void FileInfoBD::elapse_hlpr()
{
    m_isElapsed = true;
    doElapsing();
}

void FileInfoBD::cancelElapsing()
{
    m_cancelled_elapsing = true;
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->cancelElapsing();
    }
}

// collapsing-functions:
void FileInfoBD::collapse()
{
    resetCancelFlags();

    collapse_hlpr();
    emit collapsingFinished();
}
void FileInfoBD::collapse_rec()
{
    resetCancelFlags();

    collapse_hlpr();

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_collapsing)
            break;

        (*it)->collapse_rec();
    }
    emit collapsingFinished();
}
void FileInfoBD::collapse_hlpr()
{
    m_isElapsed = false;
    doElapsing();
}

void FileInfoBD::cancelCollapsing()
{
    m_cancelled_collapsing = true;
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->cancelCollapsing();
    }
}

// show-hide-hiddenFiles-functions:
void FileInfoBD::showHiddenFiles()
{
    resetCancelFlags();

    if( !m_showHiddenFiles )
    {
        showHiddenFiles_hlpr();
    }
    emit showingHiddenFilesFinished();
}
void FileInfoBD::hideHiddenFiles()
{
    resetCancelFlags();

    if(m_showHiddenFiles)
    {
        hideHiddenFiles_hlpr();
    }
    emit hidingHiddenFilesFinished();
}
void FileInfoBD::showHiddenFiles_rec()
{
    resetCancelFlags();

    showHiddenFiles_hlpr();

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_hidingHiddenFiles)
            break;

        (*it)->showHiddenFiles_rec();
    }
    emit showingHiddenFilesFinished();
}
void FileInfoBD::hideHiddenFiles_rec()
{
    resetCancelFlags();

    hideHiddenFiles_hlpr();
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_hidingHiddenFiles)
            break;

        (*it)->hideHiddenFiles_rec();
    }
    emit hidingHiddenFilesFinished();
}
void FileInfoBD::showHiddenFiles_hlpr()
{
    m_showHiddenFiles = true;
    revalFolderContent();
}
void FileInfoBD::hideHiddenFiles_hlpr()
{
    m_showHiddenFiles = false;
    revalFolderContent();
}

void FileInfoBD::cancelHidingHiddenFiles()
{
    m_cancelled_hidingHiddenFiles = true;
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->cancelHidingHiddenFiles();
    }
}
//-----------------------------------------------------

void FileInfoBD::revalFolderContent()
{
    QDir dir(m_fileInfo.absoluteFilePath());    

    QFileInfoList filesLst = ListFiles::getFilesInDirectory(dir, m_showHiddenFiles);
    m_files.clear();
    for(int i=0; i < filesLst.size(); ++i)
    {
        m_files.emplace(std::make_pair(filesLst[i].absoluteFilePath().toStdString(), filesLst[i]));
    }

    QFileInfoList foldersLst = ListFiles::getFoldersInDirectory(dir, m_showHiddenFiles);

    std::unordered_set<std::string> newFolders;

    for(int i=0; i < foldersLst.size(); ++i)
    {
        newFolders.emplace( foldersLst[i].absoluteFilePath().toStdString() );
    }

    for(auto it = m_sub_fold_names.begin(); it != m_sub_fold_names.end(); )
    {
        if( newFolders.find( it->first ) != newFolders.end() )
        {
            it->second->close();
            delete (it->second);
            it = m_sub_fold_names.erase(it);
        }else{
            ++it;
        }
    }
    for(auto it=newFolders.begin(); it != newFolders.end(); ++it)
    {
        if( m_sub_fold_names.find( *it ) != m_sub_fold_names.end() )
        {
            // bereits enthalten
        }else{
            auto sub = new FileInfoBD(QString(it->c_str()), this);
            m_sub_fold_names.emplace(std::make_pair(*it, sub));
        }
    }

    m_sub_folds.clear();
    for(auto it = m_sub_fold_names.begin(); it != m_sub_fold_names.end(); ++it)
    {
        m_sub_folds.insert(it->second);
    }

    m_subFoldsCount = foldersLst.size();
    m_filesCount = filesLst.size();
    m_contentCount = m_subFoldsCount + m_filesCount;

    emit contentHasChanged(m_fileInfo.absoluteFilePath());
}

void FileInfoBD::doElapsing()
{
    if(m_isElapsed && !m_alrLoaded){
        m_alrLoaded = true;

        revalFolderContent();
    }
}

//void FileInfoBD::removeFromParent()
//{
//    m_parent->removeFolder(m_fileInfo);
//}

//bool FileInfoBD::removeFolder(const QFileInfo& fi){
//    for(int i=0; i < m_sub_folds.size(); ++i)
//    {
//        if(m_sub_folds[i]->m_fileInfo == fi)
//        {
//            m_sub_folds[i].reset();
//            m_sub_folds.erase(m_sub_folds.begin() + i);
//            m_sub_fold_names.erase(m_sub_fold_names.begin() + i);
//            return true;
//        }
//    }
//    return false;
//}
//bool FileInfoBD::removeFolder(const QString& foldName){
//    for(int i=0; i < m_sub_folds.size(); ++i)
//    {
//        if(m_sub_folds[i]->m_fileInfo.fileName() == foldName)
//        {
//            m_sub_folds[i].reset();
//            m_sub_folds.erase(m_sub_folds.begin() + i);
//            m_sub_fold_names.erase(m_sub_fold_names.begin() + i);
//            return true;
//        }
//    }
//    return false;
//}

bool FileInfoBD::isEmpty() const
{
    return m_contentCount == 0;
}

void FileInfoBD::resetCancelFlags()
{
    m_cancelled_elapsing = false;
    m_cancelled_collapsing = false;
    m_cancelled_hidingHiddenFiles = false;
}
