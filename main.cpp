#include <QCoreApplication>
#include <QMetaType>

#include <vector>
#include <string>

#include "fileinfobd.h"
#include "fibdviewer.h"
#include "filemanager.h"
#include "filequeue.h"
#include "filemimetypehandler.h"

Q_DECLARE_METATYPE(FileInfoBD);
Q_DECLARE_METATYPE(FiBDViewer);
Q_DECLARE_METATYPE(QueueTask);
Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(std::vector<FiBDViewer>);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<FileInfoBD>();
    qRegisterMetaType<FiBDViewer>();
    qRegisterMetaType<QueueTask>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<std::vector<FiBDViewer>>();

    std::string root_path = "/home/hippo/Documents";

    FileMimeTypeHandler fmh("");
    fmh.genMimeAppInfos();

//    FileManager fi_manager(root_path);

    return a.exec();
}
