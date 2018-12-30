#ifndef FILEMIMETYPEHANDLER_H
#define FILEMIMETYPEHANDLER_H

#include <QObject>
#include <QChar>
#include <QDir>
#include <QString>
#include <QFileInfo>
#include <QFileInfoList>
#include <QImage>

#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
//#include <experimental/filesystem>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <limits.h>
#include <unistd.h>

#include "stringops.h"
#include "listfiles.h"
#include "exec.h"

struct AppInfo
{
    std::string name;
    std::string exec_cmnd;
    std::string icon_path;
    std::unordered_set<std::string> mime_types;

    AppInfo()
        : name(""),
          exec_cmnd(""),
          icon_path(""),
          mime_types(std::unordered_set<std::string>())
    {}

    AppInfo(std::string appInfoStr)
    {
        const auto& infos = StringOps::split(appInfoStr, "__|__");
        name = infos.at(0);
        exec_cmnd = infos.at(1);
        icon_path = infos.at(2);
        addMimeTypesFromVector(StringOps::split(infos.at(3), ";"));
    }

    std::string genExecCmnd(const std::vector<std::string>& args) const
    {
        std::string cmnd = exec_cmnd;
        cmnd = StringOps::replaceAll(cmnd, "%u", "");
        cmnd = StringOps::replaceAll(cmnd, "%U", "");
        cmnd = StringOps::replaceAll(cmnd, "%f", "");
        cmnd = StringOps::replaceAll(cmnd, "%F", "");
        cmnd = StringOps::trim(cmnd);

        std::string args_str = "";
        for(unsigned long i=0; i < args.size(); ++i)
        {
            args_str.append(" ");
            args_str.append(args.at(i));
        }
        cmnd.append(args_str);

        return StringOps::trim(cmnd);
    }

    void addMimeTypesFromVector(const std::vector<std::string>& mts)
    {
        mime_types.clear();
        for(auto& mt: mts)
            mime_types.insert(mt);
    }

    std::string toString() const{
        std::string s;
        std::string sep("__|__");
        s.append(name);
        s.append(sep);
        s.append(exec_cmnd);
        s.append(sep);
        s.append(icon_path);
        s.append(sep);
        unsigned long i = 0;
        for(const auto& mt: mime_types)
        {
            s.append(mt);
            if(i++ < mime_types.size()-1)
                s.append(";");
        }
        return s;
    }
};

class FileMimeTypeHandler : public QObject
{
    Q_OBJECT
public:
    explicit FileMimeTypeHandler(std::string path,
                                 QObject *parent = nullptr);

    void genMimeAppInfos() const;
signals:

public slots:
private:
    void evalMatchingApps();

    std::string getExePath() const;

    std::string getResourcesPath() const;
    std::string getMimesInfoPath() const;
    std::string getMimesInfoFilePath() const;

    std::vector<std::string> getDesktopFilesPaths() const;
    std::vector<std::string> getIconFilePaths() const;


    std::unordered_set<std::string> findIconInPathsRec(const std::string& icon_name, const std::string& path) const;
    std::string getIconPath(const std::string& icon_name) const;

    void createMimeResourceDirIfNotExistent() const;

    std::unordered_set<AppInfo> m_matching_apps;
    std::string m_mime_type;
    std::string m_path;
};

#endif // FILEMIMETYPEHANDLER_H
