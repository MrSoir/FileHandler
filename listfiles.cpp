#include "listfiles.h"


QFileInfoList ListFiles::getEntriesInDirectoryHelper(QDir dir, QDir::Filters flags)
{
    return dir.entryInfoList(flags);
}

QFileInfoList ListFiles::getFoldersInDirectory(const QDir& dir, bool showHiddenFiles)
{
    QDir::Filters filter = showHiddenFiles ? (QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden) :
                                             (QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);

    return getEntriesInDirectoryHelper(dir, filter);
}
QFileInfoList ListFiles::getFilesInDirectory(const QDir& dir, bool showHiddenFiles)
{
    QDir::Filters filter = showHiddenFiles ? (QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden) :
                                             (QDir::Files | QDir::NoDotAndDotDot);
    return getEntriesInDirectoryHelper(dir, filter);
}

//std::vector<std::string> ListFiles::getEntriesInDirectory(const std::string& path, std::function<bool(const std::string&)> f)
//{
//    std::vector<std::string> sub_pths;
//    for(const auto& entry: std::experimental::filesystem::directory_iterator(path))
//    {
//        std::string sub_path = entry.path().string();
//        if(f(sub_path))
//            sub_pths.push_back(sub_path);
//    }
//    return sub_pths;
//}

//std::vector<std::string> ListFiles::getFoldersInDirectory(const std::string &path)
//{
//    return getEntriesInDirectory(path, [](const auto& sub_path){return std::experimental::filesystem::is_directory(sub_path);});
//}

//std::vector<std::string> ListFiles::getFilesInDirectory(const std::string &path)
//{
//    return getEntriesInDirectory(path, [](const auto& sub_path){return !std::experimental::filesystem::is_directory(sub_path);});
//}
